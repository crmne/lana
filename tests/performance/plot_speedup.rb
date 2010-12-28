#!/usr/bin/env ruby

require 'logger'
require 'optparse'
require './mpi_config'

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
  # Defaults that aren't as frequently changed as to get an option
  projhome = "."
  MPIEXEC_HOSTS_FLAG = "-H"
  
  idlest_servers = %x{#{projhome}/idlest_servers.rb -s #{options[:servers].join(',')} -c #{options[:procs]} -u #{options[:user]}} if options[:same]
  (1..options[:procs]).each do |nprocs|
    idlest_servers = %x{#{projhome}/idlest_servers.rb -s #{options[:servers].join(',')} -c #{nprocs} -u #{options[:user]}} unless options[:same]
    realcommand = <<EOL
#{MPIEXEC}
#{MPIEXEC_NUMPROC_FLAG}
#{nprocs}
#{MPIEXEC_PREFLAGS}
#{MPIEXEC_HOSTS_FLAG}
#{idlest_servers.chomp}
#{options[:command]}
#{MPIEXEC_POSTFLAGS}
#{options[:cmdopts]}
EOL
    realcommand.gsub!(/ +/, ' ') # remove double spaces
    logger.info "Running #{realcommand}..."
    result = %x{#{realcommand}}
    # if no datafile were specified, search .log file in the output of
    # the process
    options[:datafiles] += result.split.grep(/.*\.log/) if options[:datafiles].empty?
  end
end

datafile = options[:datafiles].first
plotfile = datafile.split('-').first + ".png"
logger.info "Plotting #{plotfile} from #{options[:datafiles].inspect}..."

plotcode = "set term png\n"
plotcode << "set output '#{plotfile}'\n"
plotcode << "set xlabel 'Processes'\n"
plotcode << "set ylabel 'Seconds'\n"
plotcode << "plot '#{datafile}' using 3:5 with linespoints"

options[:datafiles][1..-1].each do |datafile|
  plotcode << ", '#{datafile}' using 3:5 with linespoints"
end

plotcode << "\n"

IO.popen("gnuplot", "w") { |pipe| pipe.puts plotcode }
logger.info "All done."

