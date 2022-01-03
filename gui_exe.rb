# 実際にパズルをやっているところを見せる準備をするプログラム
puts "ファイルをコピーします"

# submit.txt、problem.ppmのコピー
system('copy puzzle\x64\Debug\submit.txt puzzle_visualizer\submit.txt')
system('copy misitm_test\problem.ppm puzzle_visualizer\problem.ppm')
