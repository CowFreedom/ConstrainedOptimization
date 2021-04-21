--[[
	Authors: Devansh Rastogi & Tristan Schneidemann
]]


--loads timesteps. Assumes the timesteps are the first column
--path: path to file, filename: name of the file, comment: how comments in the file are prefixed
function load_single_file(path,filename,comment)
	file=io.open(path..filename)
	timesteps={}
	if file then
		iter=1
		for line in file:lines() do
			if string.match(line,comment) then
			else
				--numbers=line:gmatch("%S+")
				local number =line:match("%S+")
				--table.insert(timesteps,tonumber(number))
				timesteps[iter]=tonumber(number)
				--print(number)
			end	
		end
	end
	return timesteps
end

function print_timesteps(steps)
	result=""
	for i=1,#steps do
		result=result..tonumber(steps[i]).."\t"		
	end
	result=result.."\n"
	print(result)
end


function get_data(path, filename,filetype,comment,startindex)
	local timevals={}
	local keep_iterating=true
	local data={}
	local iter=1
	while keep_iterating do
		file=io.open(path..filename.."_"..tostring(startindex+iter-1)..filetype)
		if file==nil then
			keep_iterating=false
		else
			local row=1
			local omit_time=false
			data[iter]={}
			for line in file:lines() do
				if string.match(line,comment) then
				else
				data[iter][row]={}
					if omit_time==false then
						local temp=line:match("[^,]+")
						timevals[iter]=tonumber(temp)
						omit_time=true
					end
					local omit_column=true
					local counter=1
					for number in line:gmatch("[^,]+") do
						if omit_column==false then
							
							data[iter][row][counter]={}
							data[iter][row][counter]=tonumber(number)
							counter=counter+1
						else
							omit_column=false
						end
					end
					row=row+1
				end
			end
			iter=iter+1
		end
	end
	if (data==nil) then
		print("Error in get_data: Data is nil")
	end

	return timevals,data
end


--parses the data from all .csvs into a single file. The time column will be omitted (assumed to be first column in each file), so that only the "data" will be saved
--[[ format updated:
         time,posx,posy,g
         time,posx,posy,a
         time,posx,posy,k
         time,posx,posy,r
         time,posx,posy,v 
         
         time, a, g, k,r,v,x,y,z
         ]]
function get_data_sim(path, filename,filetype,comment,startindex)
	local timevals={}
	local keep_iterating=true
	local data={}
	local iter=1
	while keep_iterating do
		file=io.open(path..filename.."_"..tostring(startindex+iter-1)..filetype)
		if file==nil then
			keep_iterating=false
		else
			local row=0
			local omit_time=false
			data[iter]={}
			local linenumber= 0
			local omit_line = false
			local counter=1
			for line in file:lines() do
				if string.match(line,comment) then
				else				 
					if linenumber%5==0 then
						counter=1
						row=row+1
						data[iter][row]={}
						if omit_time==false then
							local temp=line:match("[^,]+")
							timevals[iter]=tonumber(temp)
							omit_time=true
						end
						local omit_column=true
					
						for number in line:gmatch("[^,]+") do
							if omit_column==false then
								data[iter][row][counter]={}
								data[iter][row][counter]=tonumber(number)
								counter=counter+1 --
							else
								--here time is ignored, on 1st run, then omit_col set to false
								omit_column=false
							end
						 end 
						 
					else
						local omits=3
						for number in line:gmatch("[^,]+") do
							if omits ~= 0 then
								omits= omits-1
							else
								data[iter][row][counter]=tonumber(number)
								counter=counter+1
							end
						 end 
					end
				 linenumber = linenumber+1
				end --end if
			end -- for line end
			iter=iter+1
		end --end if file==nil
	end --end while keep_iterating
	if (data==nil) then
		print("Error in get_data: Data is nil")
	end

	return timevals,data
end

--Prints 2d lua table that can be indexed like an array
function print_data(data)
	--print(#data)
	result=""
	for i= 1, #data do
		--print(#data[1])
		for j= 1, #data[1] do
			result=result..data[i][j].."\t"
		end
		result=result.."\n"
	end
	print(result)
end

--#timesteps: Array of the timestep  #data:2d array of area data per timestep #name: names of the area variables (e.g. city names) 
function write_data(path,filename,filetype,comment,timesteps,data, names)
	if (#data[1] ~= #names) then
		print("Error in write data: names and data array dimensions are wrong\n")
	end

	result=""
	result=comment.."time"
	for index, name in ipairs(names) do
		result=result.."\t"..name
	end
	result=result.."\n"
	f=io.open(path..filename..filetype,"w")
	for k=1,#timesteps do

		result=result..timesteps[k]
	
		for j=1,#data[1] do
			result=result.."\t"..data[k][j]
		end
		result=result.."\n"

	end
	f:write(result)
	f:close()
end

----------- CITIES ---------------

--FRA
function Frankfurt(data_at_time_t,col,posx, posy,density_at_time_t)
	local radius=8.89 
	local x=-126.971
	local y=-258.315
	local index=-1
	local values={}
	local iter=1

	for j=1, #data_at_time_t do
		if ((x-data_at_time_t[j][posx])*(x-data_at_time_t[j][posx])+(y-data_at_time_t[j][posy])*(y-data_at_time_t[j][posy]))<=(radius*radius) then
			index=j
			values[iter]=data_at_time_t[j][col]
			iter=iter+1
		end
	end

	local mean=0
	for i=1,#values do
		mean=mean+values[i]
	end
	if #values==0 then
		print("Error: One of the error functions has positional coordinates that cannot be found in the data\n")
		os.exit()
	end
	mean=mean/(#values)
	densityIndexFFM=17 --row index of Frankfurt in the density table. Temporary values, probably different in your sim. Index starts at 1
	return mean*3.14*radius*radius*density_at_time_t[densityIndexFFM][1]
end

--WIES
function Wiesbaden(data_at_time_t,col,posx, posy,density_at_time_t)
	--local radius=300 --testing
	local radius=8.05
	local x=-166
	local y=-240
	local index=-1
	local values={}
	local iter=1

	for j=1, #data_at_time_t do
		if ((x-data_at_time_t[j][posx])*(x-data_at_time_t[j][posx])+(y-data_at_time_t[j][posy])*(y-data_at_time_t[j][posy]))<=(radius*radius) then
			index=j
			values[iter]=data_at_time_t[j][col]
			iter=iter+1
		end
	end

	local mean=0
	for i=1,#values do
		mean=mean+values[i]
	end
	if #values==0 then
		print("Error: One of the error functions has positional coordinates that cannot be found in the data\n")
		os.exit()
	end
	mean=mean/(#values)
	densityIndexWIE=18 
	return mean*3.14*radius*radius*density_at_time_t[densityIndexWIE][1]
end

--STU
function Stuttgart(data_at_time_t,col,posx, posy,density_at_time_t)
	--local radius=300 --testing
	local radius=8.12
	local x=-78.76 
	local y=-406.635
	local index=-1
	local values={}
	local iter=1

	for j=1, #data_at_time_t do
		if ((x-data_at_time_t[j][posx])*(x-data_at_time_t[j][posx])+(y-data_at_time_t[j][posy])*(y-data_at_time_t[j][posy]))<=(radius*radius) then
			index=j
			values[iter]=data_at_time_t[j][col]
			iter=iter+1
		end
	end

	local mean=0
	for i=1,#values do
		mean=mean+values[i]
	end
	if #values==0 then
		print("Error: One of the error functions has positional coordinates that cannot be found in the data\n")
		os.exit()
	end
	mean=mean/(#values)
	densityIndexSTU=19 
	return mean*3.14*radius*radius*density_at_time_t[densityIndexSTU][1]
end

--HANN
function Hannover(data_at_time_t,col,posx, posy,density_at_time_t)
	local radius=8.061
	local x=-41.784
	local y=13.7585
	local index=-1
	local values={}
	local iter=1

	for j=1, #data_at_time_t do
		if ((x-data_at_time_t[j][posx])*(x-data_at_time_t[j][posx])+(y-data_at_time_t[j][posy])*(y-data_at_time_t[j][posy]))<=(radius*radius) then
			index=j
			values[iter]=data_at_time_t[j][col]
			iter=iter+1
		end
	end

	local mean=0
	for i=1,#values do
		mean=mean+values[i]
	end
	if #values==0 then
		print("Error: One of the error functions has positional coordinates that cannot be found in the data\n")
		os.exit()
	end
	mean=mean/(#values)
	densityIndexHANN=20
	return mean*3.14*radius*radius*density_at_time_t[densityIndexHANN][1]
end

--MUN
function Munich(data_at_time_t,col,posx, posy,density_at_time_t)
	--local radius=300 --testing
	local radius=9.94
	local x=78.84
	local y=-480.961
	local index=-1
	local values={}
	local iter=1

	for j=1, #data_at_time_t do
		if ((x-data_at_time_t[j][posx])*(x-data_at_time_t[j][posx])+(y-data_at_time_t[j][posy])*(y-data_at_time_t[j][posy]))<=(radius*radius) then
			index=j
			values[iter]=data_at_time_t[j][col]
			iter=iter+1
		end
	end

	local mean=0
	for i=1,#values do
		mean=mean+values[i]
	end
	if #values==0 then
		print("Error: One of the error functions has positional coordinates that cannot be found in the data\n")
		os.exit()
	end
	mean=mean/(#values)
	densityIndexMUN=21
	return mean*3.14*radius*radius*density_at_time_t[densityIndexMUN][1]
end

--HEINS
function Heinsberg(data_at_time_t,col,posx, posy,density_at_time_t)
	--local radius=300 --testing
	local radius=5.42
	local x=-338.146 
	local y=-151.782
	local index=-1
	local values={}
	local iter=1

	for j=1, #data_at_time_t do
		if ((x-data_at_time_t[j][posx])*(x-data_at_time_t[j][posx])+(y-data_at_time_t[j][posy])*(y-data_at_time_t[j][posy]))<=(radius*radius) then
			index=j
			values[iter]=data_at_time_t[j][col]
			iter=iter+1
		end
	end

	local mean=0
	for i=1,#values do
		mean=mean+values[i]
	end
	if #values==0 then
		print("Error: One of the error functions has positional coordinates that cannot be found in the data\n")
		os.exit()
	end
	mean=mean/(#values)
	densityIndexHEINS=22
	return mean*3.14*radius*radius*density_at_time_t[densityIndexHEINS][1]
end

--BE
function Berlin(data_at_time_t,col,posx, posy,density_at_time_t)
	--local radius=300 --testing
	local radius=16.84
	local x=209.037
	local y=15.913
	local index=-1
	local values={}
	local iter=1

	for j=1, #data_at_time_t do
		if ((x-data_at_time_t[j][posx])*(x-data_at_time_t[j][posx])+(y-data_at_time_t[j][posy])*(y-data_at_time_t[j][posy]))<=(radius*radius) then
			index=j
			values[iter]=data_at_time_t[j][col]
			iter=iter+1
		end
	end

	local mean=0
	for i=1,#values do
		mean=mean+values[i]
	end
	if #values==0 then
		print("Error: One of the error functions has positional coordinates that cannot be found in the data\n")
		os.exit()
	end
	mean=mean/(#values)
	densityIndexBE=11
	return mean*3.14*radius*radius*density_at_time_t[densityIndexBE][1]
end

----------- CITIES end ---------------


--#timesteps: timesteps of the simulation #data: contains sim data only #col which column of the data table to focus on (e.g. ngesteckte) #posx index of x position in the data array (without time column) #areafunction: table containing functions of area #densities: contains densities at various times
function tailor_data(timesteps,data,col,posx,posy,areafunctions,densities)
	nSteps=#timesteps
	nVariables=#areafunctions
	output={}
	for i=1, nSteps do
		output[i]={}
		for j=1, nVariables do
			local value=areafunctions[j](data[i],col,posx,posy,densities[i]) -- using denstiy for time step 0 as others wont load 
			output[i][j]=value
		end	
	end
	return output
end


local timesteps,data=get_data_sim("./","simdata_step",".csv","#",0)

local temp,densities=get_data("./","density_step",".csv","T",0)


areas={}
areas[1]=Hannover
areas[2]=Heinsberg
areas[3]=Frankfurt
areas[4]=Wiesbaden
areas[5]=Stuttgart
areas[6]=Munich
areas[7]=Berlin


--dont forget: Tables in Lua often start with index 1 instead 0
data_a=tailor_data(timesteps,data,4,1,2,areas,densities)
data_v=tailor_data(timesteps,data,7,1,2,areas,densities)


--[[print("Parsed timesteps")
print_timesteps(timesteps)
print("Parsed data")
print_data(data[2]) --prints data at time 0 (don't forget: Lua indices by convention start at 1)
print("Parsed densities")
print_data(densities[1]) --prints density at time 0 (don't forget: Lua indices start at 1)]]

print("Writing integrated Data to file..")
write_data("./","output_v",".csv","#",timesteps,data_v,{"Hannover","Heinsberg","Frankfurt","Wiesbaden", "Stuttgart","Munich","Berlin"})

write_data("./","output_a",".csv","#",timesteps,data_a,{"Hannover","Heinsberg","Frankfurt","Wiesbaden", "Stuttgart","Munich","Berlin"})

print("Data has been written to file") 
