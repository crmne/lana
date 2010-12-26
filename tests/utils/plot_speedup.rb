#!/usr/bin/env ruby

require 'logger'
require 'optparse'
require './mpi_config'

logger = Logger.new STDERR
options = { :logger => logger, :user => ENV['USER'], :procs => 5, :cmdopts => "--log_level=nothing", :same => false }

opts = OptionParser.new do |opts|
  opts.banner = "Usage: #{$0} [options]"
  opts.on('-s', '--servers X,Y,Z', Array, "REQUIRED: Comma separated list of servers to test") do |list|
    options[:servers] = list
  end
  opts.on('-c', '--command STRING', "REQUIRED: The command to run") do |command|
    options[:command] = command
  end
  opts.on('-C', '--command-options "STRING"', "Pass the quoted string to the command") do |cmdopts|
    options[:cmdopts] = cmdopts
  end
  opts.on('-p', '--procs NUMBER', Integer, "Max number of processes (default = 5)") do |num|
    options[:procs] = num
  end
  opts.on('-S', '--same-servers', "Use always the same servers") do |same|
    options[:same] = true
  end
  opts.on_tail("-h", "--help", "Show this message") do
    puts opts
    exit
  end
end

opts.parse!
abort "Error: Please specify all the required options.\n\n#{opts}" unless (options[:servers] and options[:command])


projhome = "."
datafile = "#{File.basename options[:command]}.dat"
plotfile = "#{File.basename options[:command]}.png"
data = File.open datafile, 'w'
MPIEXEC_HOSTS_FLAG = "-H"

if options[:same]
  idlest_servers = %x{#{projhome}/idlest_servers.rb -s #{options[:servers].join(',')} -c #{options[:procs]} -u #{options[:user]}}
end

(1..options[:procs]).each do |nprocs|
  unless options[:same]
    idlest_servers = %x{#{projhome}/idlest_servers.rb -s #{options[:servers].join(',')} -c #{nprocs} -u #{options[:user]}}
  end
  realcommand = "#{MPIEXEC} #{MPIEXEC_NUMPROC_FLAG} #{nprocs} #{MPIEXEC_PREFLAGS} #{MPIEXEC_HOSTS_FLAG} #{idlest_servers.chomp} #{options[:command]} #{MPIEXEC_POSTFLAGS} #{options[:cmdopts]}".gsub(/ +/, ' ')
  logger.info "Running #{realcommand}..."
  result = %x{#{realcommand}}
  data << result
end
data.close

logger.info "Plotting #{plotfile}..."

plotcode = <<EOL
set term png
set output "#{plotfile}"
plot "#{datafile}" every 2 using 3:5 with linespoints, "#{datafile}" every 2::1 using 3:5 with linespoints
EOL

IO.popen("gnuplot", "w") { |pipe| pipe.puts plotcode }
logger.info "All done."

