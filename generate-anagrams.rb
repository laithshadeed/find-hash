#!/usr/bin/env ruby

# Non-computable solution !

require "digest"

def permuteHelper(known_words, s, chosen)
  if s.empty?
    # Skip if not in the known words
    chosen.split(" ").each do |w|
      return unless known_words.key?(w)
    end

    # Get md5
    md5 = Digest::MD5.hexdigest chosen

    # Check md5
    if md5 == "e4820b45d2277f3844eac66c903e84be" ||
        md5 == "23170acc097c24edb98fc5488ab033fe" ||
        md5 == "665e5bcb0c20062fe8abaaf4628bb154"

      puts "#{chosen} #{md5}"
    end
  else
    i = 0
    while i < s.length do
      # choose char
      c = s[i]
      chosen += c
      s.slice!(i)

      # explore
      permuteHelper(known_words, s, chosen)

      # un-choose
      s.insert(i, c)
      chosen.chop!
      i += 1
    end
  end
end

def permuteMain(known_words, s)
  permuteHelper(known_words, s, "")
end

anagram = "poultry outwits ants"
known_words = {}
IO.foreach(ARGV[0]).each do |w|
  w.chomp!
  known_words[w] = known_words.key?(w) ? known_words[w] + 1 : 1;
end

permuteMain(known_words, anagram)
