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
]

ELFS.each_with_index{ |elf, i|
  n = i + 1
  puts "##{n}: #{elf}"
  some_failed = false
  Options.each_with_index{ |opt, j|
    m = j + 1
    print "#{m}:\"#{opt}\"...";
    suffix = "#{elf}_#{opt}.txt"
    real_file = "results/real_#{suffix}.txt"
    mine_file = "results/mine_#{suffix}.txt"
    `nm #{opt} #{elf} > #{real_file}; ../ft_nm #{opt} #{elf} 2>/dev/null > #{mine_file}`
    diff = `diff #{real_file} #{mine_file}`
    ok = diff == ""
    some_failed = some_failed || !ok
    print "#{ok ? "ok" : "KO"} "
  }
  puts ""
  puts some_failed ? "[KO]" : "[ok]"
}

