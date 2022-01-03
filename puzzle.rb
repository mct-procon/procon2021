# 画像復元後にパズルを自動で解く(puzzleを実行させる)

# 読み込み
f = File.open('misitm_test\output2.txt', "r")
s = f.read
f.close

# 入力ファイル
f = File.open('puzzle\x64\Debug\input.txt', "w")
f.puts("8\n0\n0\n")
f.print(s)
f.close

# 実行
system('puzzle\x64\Debug\procon_2021_pazzule.exe < puzzle\x64\Debug\input.txt')
