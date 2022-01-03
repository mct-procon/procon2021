# problemsのファイルを移動させる
if ARGV.size == 0
  puts "ファイル名を指定して"
  exit
end
ppm = File.binread("problems/#{ARGV[0]}.ppm")
File.binwrite("misitm_test/problem.ppm", ppm)
