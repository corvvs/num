ELFS = ARGV


Options = [
  "",
  "-a",
  "-r",
  "-g",
  "-u",
  "-p",
  "-au",
  "-ap",
  "-ag",
  "-ar",
  "-ua",
  "-pa",
  "-ga",
  "-ra",
  "-up",
  "-ug",
  "-ur",
  "-pu",
  "-gu",
  "-ru",
  "-gu",
  "-gr",
  "-ug",
  "-rg",
  "-a -g",
  "-a -a",
  "-argup",
  "-ar -up",
]

some_elf_failed = false

ELFS.each_with_index{ |elf, i|
  n = i + 1
  puts "##{n}: #{elf}"
  some_failed = false
  Options.each_with_index{ |opt, j|
    m = j + 1
    dir = "results/#{elf}_#{opt.split(" ").join("_")}"
    real_file = "#{dir}/real.txt"
    mine_file = "#{dir}/mine.txt"
    diff_file = "#{dir}/compare.diff"

    `mkdir -p #{dir}; nm #{opt} #{elf} > #{real_file}; ../ft_nm #{opt} #{elf} 2>/dev/null > #{mine_file}`
    diff = `diff #{real_file} #{mine_file}`

    ok = diff == ""
    some_failed = some_failed || !ok
    if ok then
      print "\e[32m.\e[0m"
    else
      print "\e[31m#{m}:\"#{opt}\"\e[0m"
    end
    print " "
    if ok then
      `rm -rf #{dir}`
    else
      File.open(diff_file, "w") { |f| f.puts diff }
    end
  }

  puts ""
  puts "#{some_failed ? "\e[31m[KO]\e[0m" : "\e[32m[ok]\e[0m"} #{elf}"
  puts ""
  some_elf_failed = some_elf_failed || some_failed
}

exit (some_elf_failed ? 1 : 0)

