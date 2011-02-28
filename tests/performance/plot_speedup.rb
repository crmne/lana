#!/usr/bin/env ruby

require 'rubygems'
require 'logger'
require 'optparse'
require 'active_support'

logger = Logger.new STDERR
options = { :logger => logger, :user => ENV['USER'], :procs => 5, :cmdopts => "", :same => false, :plotonly => false, :datafiles => [] }

opts = OptionParser.new do |opts|
  opts.banner = "Usage: #{$0} [options]"
  opts.on('-s', '--servers X,Y,Z', Array, "REQUIRED: Comma separated list of servers", "to test") do |list|
    options[:servers] = list
  end
  opts.on('-c', '--command PATH', "REQUIRED: The command to run") do |command|
    options[:command] = command
  end
  opts.on('-d', '--datafiles X,Y,Z', Array, "Datafiles to plot") do |datafiles|
    options[:datafiles] = datafiles
  end
  opts.on('-C', '--command-options "STRING"', "Pass the quoted string to the command") do |cmdopts|
    options[:cmdopts] = cmdopts
  end
  opts.on('-p', '--procs NUMBER', Integer, "Max number of processes (default = 5)") do |num|
    options[:procs] = num
  end
  opts.on('-S', '--same-servers', "Always use the same servers, instead", "of selecting the best servers each time") do |same|
    options[:same] = true
  end
  opts.on('-P', '--plot-only', "Just plots the graph and exits", "Doesn't require -s or -c, but", "REQUIRES -d, --datafiles") do |plot|
    options[:plotonly] = true
  end
  opts.on_tail("-h", "--help", "Show this message") do
    puts opts
    exit
  end
end

opts.parse!
abort "Error: Please specify all the required options.\n\n#{opts}" unless (options[:servers] and options[:command]) or (options[:plotonly] and not options[:datafiles].empty?)

unless options[:plotonly]
  require './mpi_config'
  # Defaults that aren't as frequently changed as to get an option
  projhome = "."
  MPIEXEC_HOSTS_FLAG = "-H"
  
  idlest_servers = %x{#{projhome}/idlest_servers.rb -s #{options[:servers].join(',')} -c #{options[:procs]} -u #{options[:user]}} if options[:same]

  (4..options[:procs]).step(2).each do |nprocs|
    idlest_servers = %x{#{projhome}/idlest_servers.rb -s #{options[:servers].join(',')} -c #{nprocs} -u #{options[:user]}} unless options[:same]
    realcommand = "#{MPIEXEC} #{MPIEXEC_NUMPROC_FLAG} #{nprocs} #{MPIEXEC_PREFLAGS} #{MPIEXEC_HOSTS_FLAG} #{idlest_servers.chomp} #{options[:command]} #{MPIEXEC_POSTFLAGS} #{options[:cmdopts]}"
    realcommand.gsub!(/ +/, ' ') # remove double spaces
    logger.info "Running #{realcommand}..."
    result = %x{#{realcommand}}
    # if no datafile were specified, search .log file in the output of
    # the process
    options[:datafiles] += result.split.grep(/.*\.log/) if options[:datafiles].empty?
  end
end

def plot logs, image, logger
  logger.info "Plotting #{image} from #{(logs.map {|x| x.file}).inspect}..."

  first = logs.first
  plotcode = "set term pdf\n"
  plotcode << "set output '#{image}'\n"
  plotcode << "set xlabel 'Processes'\n"
  plotcode << "set ylabel 'Seconds'\n"
  plotcode << "plot '#{first.file}' using 3:5 with linespoints title '#{first.graph}-#{first.bidi} #{first.nodes} nodes, #{first.edges} edges'"
  logs[1..-1].each do |log|
    plotcode << ", '#{log.file}' using 3:5 with linespoints title '#{log.graph}-#{log.bidi} #{log.nodes} nodes, #{log.edges} edges'"
  end
  plotcode << "\n"

  IO.popen("gnuplot", "w") { |pipe| pipe.puts plotcode }
end

Log = Struct.new :alg, :graph, :bidi, :nodes, :edges, :file
logs = options[:datafiles].map do |x|
  s = x.split('-')
  b = s[1].gsub(/.*([DB]i)graph/, '\1')
  e = s[3].delete('e').to_i
  e = 27811816 if e == 28106778
  e = 55623632 if e == 56213556
  Log.new s[0], s[1].gsub(/[DB]igraph/,''), b, s[2].delete('n').to_i, e, x
end

logs.group_by(&:alg).each do |algo, l|
  l.group_by(&:graph).each do |graph, ll| # same graph, different size
    plot(ll, "#{algo}-#{graph}.pdf", logger)
  end
  l.group_by(&:nodes).each do |nodes, ll| # same nodes, different graph, different edges
    plot(ll, "#{algo}-n#{nodes}.pdf", logger)
  end
  l.group_by(&:edges).each do |edges, ll| # same edges, different nodes, different graph
    plot(ll, "#{algo}-e#{edges}.pdf", logger)
  end
end

logger.info "All done."
