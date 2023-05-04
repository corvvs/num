ELFS = `echo malloc/objs/*`.split(/\s+/)
ERRORS = ["xxx", "yyy"]

some_failed = false
FILES = (ELFS + ERRORS).shuffle
(1...FILES.size).each{ |n|
  args = FILES[0...n] * " "
  dir = "results/multiple_#{n}"
  real_file = "#{dir}/real.txt"
  mine_file = "#{dir}/mine.txt"
  diff_file = "#{dir}/compare.diff"
  `mkdir -p #{dir}; cp /usr/bin/nm mm; ./mm #{args} > #{real_file} 2>&1; cp ../ft_nm mm; ./mm #{args} > #{mine_file} 2>&1`
  diff = `diff -u #{real_file} #{mine_file}`

  ok = diff == ""
  some_failed = some_failed || !ok
  if ok then
    print "\e[32m.\e[0m"
  else
    print "\e[31m\"#{args}\"\e[0m"
  end
  print " "
  if ok then
    `rm -rf #{dir}`
  else
    File.open(diff_file, "w") { |f| f.puts diff }
  end
}

puts ""
puts "#{some_failed ? "\e[31m[KO]\e[0m" : "\e[32m[ok]\e[0m"}"
puts ""

