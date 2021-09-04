# ランダマイザ
# いるもの ruby 3.0 [x64-mingw32], ImageMagick 6.9.12-19 Q16 x64, RMagick 4.2.2
# 実行 ruby r.rb [ファイル名] [x軸の分割数]
# 特殊なオプション [ファイル名] => :a(すべてのファイル), :l(まだ実行されたことがないファイル)
# 特殊なオプション [x軸の分割数] => :r(ランダム, ファイルごとに変わる)
# 例) ruby r.rb :a :r => すべてのファイルをランダムな分割数でランダマイズ

require "rmagick"
include Magick

# 1番目の実行時オプション(元画像)
if ARGV[0].nil?
    files = Dir.glob("./input/*") # 指定しなければすべて
elsif ARGV[0] == ":a"
    files = Dir.glob("./input/*") # :aならばすべて
elsif ARGV[0] == ":l"
    # :lならばまだ出力が存在しないファイルのみ
    # いったんすべて取得し, output/picturesに出力が存在していたら除外する
    files = Dir.glob("./input/*")
    exist = Dir.glob("./output/pictures/*").map{ |file| File.basename(file, ".ppm") }
    files.delete_if{ |file| exist.include?(File.basename(file, ".*")) }
elsif File.file?("./input/#{ARGV[0]}")
    files = Dir.glob("./input/#{ARGV[0]}") # ファイル名が指定され, かつ存在していた場合当該ファイルを読み込む
else
    puts "Error: File is not found" # ファイルが見つからなければエラーを出力
    exit
end

# 2番目の実行時オプション(x軸の分割数)
if ARGV[1].nil?
    sep_x = 5 # 指定しなければすべて5
elsif ARGV[1] != ":r" # :rならばスルー, ファイルごとに変化するのでそのときランダムに決める
    # 入力が数字だけで, かつ分割数の上限と下限を満たしているか確認
    # 0から9以外の文字列がマッチするか, 制限を満たしていなかった場合エラーを出力
    if ARGV[1] =~ /[^0-9]/ or ARGV[1].to_i < 2 or ARGV[1].to_i > 16
        puts "Error: Invalid value"
        exit
    else
        sep_x = ARGV[1].to_i
    end
end

for idx_file in 0...files.size # 対象のファイルの数だけ繰り返す
    sep_x = rand(2..16) if ARGV[1] == ":r" # x軸の分割数をランダムに決める, :rが指定されているときのみ

    input = ImageList.new("#{files[idx_file]}") # 元画像のImageList
    filename = File.basename(files[idx_file], ".*") # 拡張子を除いたファイル名
    tilesize = input.columns / sep_x # 断片画像のサイズ(px)
    sep_y = input.rows / tilesize # y軸の分割数
    sep = sep_x * sep_y # 断片画像の個数

    resize = input.resize(tilesize * sep_x, tilesize * sep_y) # 分割できるように元画像をリサイズ
    resize.write("./resize/#{filename}.ppm") # リサイズしたものを書き出す

    tiles = [] # 断片画像をいったん放り込む配列
    output = ImageList.new() # 出力用のImageList
    random = (0...sep).to_a.shuffle # 断片画像の位置を決める, 出力画像のi番目の断片はもとはrandom[i]番目にあった
    # 断片画像の回転を決める, 出力画像のi番目の断片は90*rotate[i]度回転している
    # 左上の断片は回転させない
    rotate = sep.times.map{ |i| (i == 0) ? 0 : rand(4) }
    # 元画像を分割して断片画像を作る
    for y in 0...sep_y
        for x in 0...sep_x
            tiles.push(resize.crop(tilesize * x, tilesize * y, tilesize, tilesize))
        end
    end

    # 断片画像をrandomに従って並び替える
    # Imageオブジェクトはcrop後もその断片画像が全体のどこに位置していたかを記憶している(page要素のRectangle)
    # したがってrandom[i]番目のRectangleの座標をi番目の座標に書き換えてから再結合させればよい
    for i in 0...sep
        tiles[random[i]].rotate!(90 * (rotate[i])) # 回転
        tiles[random[i]].page = Rectangle.new(tilesize, tilesize, tilesize * (i % sep_x), tilesize * (i / sep_x)) # (width, height, x, y)
        output.push(tiles[random[i]]) # 出力のImageListに画像を追加
    end

    # 上で書き換えたRectangle通りに画像が結合される
    output = output.mosaic

    # ヘッダのコメントをフォーマット通りに書き換える
    blob = output.to_blob{self.format = "ppm"}.split("\n") # 形式をPPMとして画像のバイナリデータを取得
    # 0番目の要素"P6"を除いて, 1番目の要素から初めて行頭に数字が現れたところの手前までがコメントなので, その部分をスライスして削除
    blob.slice!(1, blob.index{ |x| x[0] =~ /[1-9]/ } - 1)
    blob.insert(1, "# #{sep_x} #{sep_y}", "# #{rand(2..128)}", "# #{rand(1..500)} #{rand(1..100)}") # フォーマット通りに記述

    # バイナリデータとして出力
    IO.binwrite("./output/pictures/#{filename}.ppm", blob.join("\n"))

    # 位置と回転の情報をテキストファイルに出力
    File.open("./output/texts/#{filename}.txt", "w") do |f|
        f.puts "#{sep_x} #{sep_y}\n"
        f.puts random.map{ |e| e.to_s.rjust(2, "0") }.each_slice(sep_x).map{|e| e.join(" ") }.join("\n")
        f.puts "\n"
        f.puts rotate.map{ |e| e.to_s.rjust(2, "0") }.each_slice(sep_x).map{|e| e.join(" ") }.join("\n")
    end

    puts "Randomize: #{filename}.ppm (#{sep_x}, #{sep_y})"
end

puts "Randomized #{files.size} file#{ (files.one?) ? () : ("s") }."