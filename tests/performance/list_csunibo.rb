#!/usr/bin/env ruby
# Prints a comma separated list of servers in the cs.unibo.it cluster
require 'net/http'
require 'uri'

URL = 'http://www.cs.unibo.it/servizi/dept/admin/cluster/lab_linux/ssh.html'

def get_servers(url)
  Net::HTTP.get(URI.parse(url)).scan(/[a-z]+\.cs\.unibo\.it/) - [ 'ssl.cs.unibo.it', 'calaf.cs.unibo.it', 'susanna.cs.unibo.it', 'giovanna.cs.unibo.it', 'edmondo.cs.unibo.it', 'ludovico.cs.unibo.it', 'kurwenal.cs.unibo.it' ]
end

puts get_servers(URL).join(',')
