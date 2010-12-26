#!/usr/bin/env ruby

require 'logger'
require 'optparse'
require './mpi_config'

logger = Logger.new STDERR
options = { :logger => logger, :user => ENV['USER'], :procs => 5 }

opts = OptionParser.new do |opts|
  opts.banner = "Usage: #{$0} [options]"
  opts.on('-s', '--servers X,Y,Z', Array, "REQUIRED: Comma separated list of servers to test") do |list|
    options[:servers] = list
  end
  opts.on('-c', '--command STRING', "REQUIRED: The command to run") do |command|
    options[:command] = command
  end
  opts.on('-C', '--command-options "STRING"', "Pass this quoted string to command") do |cmdopts|
    options[:cmdopts] = cmdopts
  end
  opts.on('-p', '--procs NUMBER', Integer, "Max number of processes (default = 5)") do |num|
    options[:procs] = num
  end
  opts.on_tail("-h", "--help", "Show this message") do
    puts opts
    exit
  end
end

opts.parse!
abort "Error: Please specify all the required options.\n\n#{opts}" unless (options[:servers] and options[:command])


projhome = "."
datafile = File.new "#{File.basename options[:command]}.dat", 'w'
MPIEXEC_HOSTS_FLAG = "-H"

(1..options[:procs]).each do |nprocs|
  idlest_servers = %x{#{projhome}/idlest_servers.rb -s #{options[:servers].join(',')} -c #{nprocs} -u #{options[:user]}}
  realcommand = "#{MPIEXEC} #{MPIEXEC_NUMPROC_FLAG} #{nprocs} #{MPIEXEC_PREFLAGS} #{MPIEXEC_HOSTS_FLAG} #{idlest_servers.chomp} #{options[:command]} #{MPIEXEC_POSTFLAGS} #{options[:cmdopts]}".gsub(/ +/, ' ')
  logger.info "Running #{realcommand}..."
  result = %x{#{realcommand}}
  datafile << result
end

