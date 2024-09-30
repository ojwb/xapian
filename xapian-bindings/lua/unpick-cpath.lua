require("package")
so = string.lower(arg[1])
for e in string.gmatch(string.gsub(package.cpath,"\\","/"), "[^;]*") do
  l,s = string.match(e, "^(/.*)/%?(%.%a+)$")
  if l and string.lower(s) ~= ".lua" then
    if so == "" or string.lower(s) == so then
      print(l..";"..s)
      break
    end
  end
end
