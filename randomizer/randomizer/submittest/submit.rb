# 提出テスト
# コマンドライン引数に"get"か"post"を指定
# 提出時は./submit.txtの中身をpostする

require 'net/http'
require 'uri'

if ARGV[0] == 'get'
  url = 'https://procon32-pre.kosen.work/problem.ppm'
  res = Net::HTTP.get_response(URI.parse(url))
  puts "#{res.code} \"#{res.message}\""
  if res.is_a?(Net::HTTPSuccess)
    File.open('problem.ppm', 'w+b') do |file|
      file.write(res.body)
    end
  end
elsif ARGV[0] == 'post'
  token = '60903ba069865cf2be2d22d1e58bb2a9e6fdc7275b0b5e9d6b0038118d28d2e9'
  url = 'https://procon32-pre.kosen.work'
  data = ''
  File.open('submit.txt', 'r') do |file|
    res = Net::HTTP.post(URI.parse(url), file.read, { 'procon-token' => token })
    puts "#{res.code} \"#{res.message}\""
    puts res.read_body
  end
else
  puts 'Error: Invalid option'
end