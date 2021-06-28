#ヘッダの編集テスト

require "rmagick"
include Magick
=begin
bin = IO.binread("./output/pictures/infinite_rook_r - コピー.ppm").split("\n")
bin[1] = "#test"
bin.insert(2, "#hello, PPM!", "#Hi!")

File.open("./test.ppm", "wb"){ }#|dup| IO.binwrite(dup, bin.join("\n")) }
=end
i = ImageList.new("./output/pictures/infinite_rook_r.ppm")
blob = i.to_blob.split("\n")
com = blob.index{ |x| x[0] =~ /[1-9]/ }
blob.slice!(1, com - 1)
blob.insert(1,"#test!!","#hello, world")
IO.binwrite("./test.ppm", blob.join("\n"))