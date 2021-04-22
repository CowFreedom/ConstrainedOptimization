function load_single_file(path,filename,comment, sep)
    local keep_iterating=true
    local data={}
    local row= 0
    file=io.open(path..filename, "rb")
    if file==nil then keep_iterating=false
    else
        --data[row]={}
        for line in file:lines() do
            if string.match(line,comment) then
            else
                data[row]={}
                local position =1
                for number in line:gmatch("[^\t]+") do
                  local temp = number
                  --print("temp="..temp)
                  table.insert(data[row],position,tonumber(number))
                  --data[row]=tonumber(temp)
                  position = position+1
                end
                --print(line)
            end
            row=row+1
        end
    end
    return data
end

--parameters: c*f(t)+k
-- add a multiplicative and additional factor to the output.
-- defaults are c=1 and k=0
function additional_para(data,c,k)
  local rows= #data
  local cols=#data[1]
  
  for i=1,rows do
    for j=2,cols do
      data[i][j]= (c * data[i][j]) + k
    end
  end
 
 return data  
end

function write_final_data(path,filename,filetype, data, areas)
  result = "#time"
  for i,v in ipairs(areas) do
    result= result.."\t".. v
  end
  result = result.."\n"
  file = io.open(path..filename..filetype, "w")
  for i=1, #data do
    result = result ..data[i][1] .."\t"
    for j=2,#data[1] do
      result=result..data[i][j].."\t"
    end
    result=result.."\n"
  end
  
  --print(result)
  file:write(result)
  file:close()
end


out_a= load_single_file("./","output_a.csv","#","\t")
out_v= load_single_file("./","output_v.csv","#","\t")

data_a=additional_para(out_a,v_c,v_k)
data_v=additional_para(out_v,v_c,v_k)

write_final_data("./","out_a_final",".csv",data_a,{"Hannover","Heinsberg","Frankfurt","Wiesbaden", "Stuttgart","Munich","Berlin"})
write_final_data("./","out_v_final",".csv",data_v,{"Hannover","Heinsberg","Frankfurt","Wiesbaden", "Stuttgart","Munich","Berlin"})
