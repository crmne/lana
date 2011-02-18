#!/usr/bin/env ruby
# Prints a comma separated list of servers in the cs.unibo.it cluster
require 'net/http'
require 'uri'
require 'rexml/document'
require 'optparse'

URL = 'http://www.cs.unibo.it/servizi/dept/admin/cluster/lab_linux/ssh.html'

def get(url)
  Net::HTTP.get(URI.parse(url))
end

def ercolani(document)
  REXML::XPath.match(document, '//div/table[1]/tbody/tr/td').map { |el| el.text }
end

def ranzani(document)
  REXML::XPath.match(document, '//div/table[2]/tbody/tr/td').map { |el| el.text }
end

options = { :lab => :all }

OptionParser.new do |opts|
  opts.banner = "Usage: #{$0} [options]"
  opts.on('-e', '--ercolani', "Return only Ercolani's workstations") { options[:lab] = :ercolani }
  opts.on('-r', '--ranzani', "Return only Ranzani's workstations") { options[:lab] = :ranzani }
  opts.on_tail("-h", "--help", "Show this message") do
    puts opts
    exit
  end
end.parse!

doc = REXML::Document.new(get(URL))

case options[:lab]
when :all
  puts (ercolani(doc) | ranzani(doc)).join(',')
when :ercolani
  puts ercolani(doc).join(',')
when :ranzani
  puts ranzani(doc).join(',')
end