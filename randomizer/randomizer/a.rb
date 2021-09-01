#回答への返信

filename = "numtest"

if not File.file?("./output/pictures/#{filename}.ppm")
    puts "Error: File is not found"
    exit
end

movelist = { U: [0, -1], D: [0, 1], L: [-1, 0], R: [1, 0] }
wrong_p = 0
wrong_r = 0

sep, place, rotate = File.read("./output/texts/#{filename}.txt").split("\n\n").map{ _1.split.map(&:to_i) }
seps = sep[0] * sep[1]
field = place.zip(rotate).each_slice(sep[0]).to_a

rotin = gets.chomp.chars.map(&:to_i)

if rotin.size != seps
    puts "Error: Invalid rotation format"
    exit
end

line = gets.to_i

line.times do
    tile = gets.chomp.chars.map{ _1.to_i(16) }
    swaptime = gets.to_i
    swap = gets.chomp.chars.map(&:to_sym)
    swap.each do |s|
        move = movelist[s]
        if move.nil?
            puts "Error: Invalid swapping format"
            exit
        end
        befx, befy = tile[0], tile[1]
        aftx, afty = (tile[0] + move[0]) % sep[0], (tile[1] + move[1]) % sep[1]
        field[befy][befx], field[afty][aftx] = field[afty][aftx], field[befy][befx]
        tile = [aftx, afty]
    end
end

wrong_p = field.map{ _1.map(&:first) }.flatten.zip(Array(0...seps)).count{ |x, y| x != y }
wrong_r = field.map{ _1.map(&:last) }.flatten.zip(rotin.flatten).count{ |x, y| (x + y) % 4 != 0 }

puts "ACCEPTED #{wrong_p} #{wrong_r}"