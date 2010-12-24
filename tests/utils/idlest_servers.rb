#!/usr/bin/env ruby
# Copyright (c) 2010 Carmine Paolino <me@iflipbits.com>
# Distributed under the MIT License. See the accopanying file LICENSE.txt
#
# Choose less busy servers by number of cpus needed

require 'rubygems'
begin
  require 'highline'
  require 'net/ssh'
rescue LoadError, NameError
  warn <<EOM
Please install highline and net-ssh first:
$ gem install highline net-ssh
EOM
  exit
end
require 'logger'
require 'optparse'

def password_prompt(prompt)
  HighLine.track_eof = false
  Thread.exclusive { @password ||= HighLine.new.ask(prompt) { |q| q.echo = false } }
end

def connect(server, options={}, &block)
  methods = [ %w(publickey hostbased), %w(password keyboard-interactive) ]
  password = nil
  user = options[:user]
  timeout = options[:timeout]

  begin
    ssh_options = (options[:ssh_options] || {}).merge(
                                                      :password => password,
                                                      :auth_methods => methods.shift,
                                                      :timeout => timeout
                                                      )

    Net::SSH.start server, user, ssh_options do |ssh|
      yield ssh
    end
  rescue Net::SSH::AuthenticationFailed
    raise if methods.empty?
    password = password_prompt "Password for #{user}@#{server}: "
    retry
  end
end

def select_less_busy_cpus(options={})
  logger = options[:logger]
  threads = []
  options[:servers].each do |server|
    threads << Thread.new do
      begin
        Thread.current[:server] = server
        connect(server, options) do |ssh|
          uptime = ssh.exec! 'uptime'
          Thread.current[:load_average] = uptime.scan(/\d\.\d{2}/).map { |x| x.to_f }
          Thread.current[:cpus] = ssh.exec!('cat /proc/cpuinfo | grep processor | wc -l').to_i # warning: works only on Linux
        end
      rescue Exception => e
        logger.warn "#{server}: '#{e.class}' #{e.message}"
        Thread.current.exit
      end
    end
  end
  machines = []
  threads.each do |x|
    x.join
    machines << { :server => x[:server], :load_average => x[:load_average], :cpus => x[:cpus] } if x[:server] and x[:cpus]
  end
  machines = machines.sort_by { |x| x[:load_average] }.reverse
  selected = []
  while (cpus_got = selected.inject(0) { |sum, n| sum + n[:cpus] }) < options[:cpus]
    if machines.empty?
      logger.warn "Not enough available servers to get #{options[:cpus]} CPUs. I was able to get only #{cpus_got}."
      break
    end
    selected << machines.pop
  end


  logger.info "Selected: " + selected.inspect
  return selected.map { |x| x[:server] }
end

logger = Logger.new STDERR # allowing pipes and other cli wizardry
options = { :logger => logger, :user => ENV['USER'], :timeout => 3 }

opts = OptionParser.new do |opts|
  opts.banner = "Choose the most idle servers by number of CPUs needed.\nUsage: #{$0} [options]"
  opts.on('-s', '--servers X,Y,Z', Array, "REQUIRED: Comma separated list of servers") do |list|
    options[:servers] = list
  end
  opts.on('-c', '--cpus NUMBER', Integer, "REQUIRED: Number of cpus you need") do |num|
    options[:cpus] = num
  end
  opts.on('-t', '--timeout SECONDS', Integer, "Timeout for ssh connections") do |sec|
    options[:timeout] = sec
  end
  opts.on('-u', '--user NAME', "Use this username while connecting") do |name|
    options[:user] = name
  end
  opts.on_tail("-h", "--help", "Show this message") do
    puts opts
    exit
  end
end

opts.parse!
unless (options[:servers] and options[:cpus])
  puts "Error: Please specify all the required options."
  puts
  puts opts
  exit 1
end

logger.info "Requested #{options[:cpus]} cpus by #{options[:user]}"
logger.info "Testing servers #{options[:servers].inspect}"
logger.info "SSH timeout: #{options[:timeout]} seconds"

puts select_less_busy_cpus(options).join(',')
