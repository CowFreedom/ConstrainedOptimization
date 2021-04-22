--[[ Sept 2020 01.09 till Oct 10.10.20
setting inital values for Frankfurt, Wiesbaden, Stuttgart, Munich,Hannover,Berlin and Heinsberg. 
]]

local corona_DE_7Cites ={}


-- Some GLOBALs
local MyDefaultDiffusion = 1e+0 -- km^2 /day

-----------------------------------------
-- D) Initial values ("Anfangswerte")
-----------------------------------------

-- using real/approx radii -- areas of cities defined by radii pi*r2
local cities = {
   
   --{x1,x2,density, r2}
   Frankfurt={-125.856, -257.983, 3074,79.03} ,
   Wiesbaden={-166, -240, 1366,64.80 },
   Stuttgart={-78, -406.635, 3100,66.0 },
   Munich={78.94, -480.961, 4800, 98.89},
   Hannover={-41.784, 13.7585, 2630, 64.98},
   Berlin={209.037, 15.913, 4090,283.65 },
   Heinsberg={-338.146, -151.782, 460 ,29.33},
   
}

-- gesunde based on (population - angesteckte), and rest of germany is 1.0 
-- rates calculated by pop. of city 
function InitialinSubsetsG(x,y,t,si)
   value = 1.0
   if (si == 16 or si ==5 ) then
     local r2_f = (x-cities.Frankfurt[1])*(x-cities.Frankfurt[1])+(y-cities.Frankfurt[2])*(y-cities.Frankfurt[2])
     if (r2_f < cities.Frankfurt[4]) then value =  0.996487987 end
   end
   
   if(si == 17 or si == 5) then
     local r2_w = (x-cities.Wiesbaden[1])*(x-cities.Wiesbaden[1])+(y-cities.Wiesbaden[2])*(y-cities.Wiesbaden[2])
     if (r2_w < cities.Wiesbaden[4]) then value =0.996883012 end
   end 

   if (si == 18 or si ==7 ) then
     local r2_f = (x-cities.Stuttgart[1])*(x-cities.Stuttgart[1])+(y-cities.Stuttgart[2])*(y-cities.Stuttgart[2])
     if (r2_f < cities.Stuttgart[4]) then value =  0.996701479 end
   end
  
   if(si == 20 or si == 8) then
     local r2_w = (x-cities.Munich[1])*(x-cities.Munich[1])+(y-cities.Munich[2])*(y-cities.Munich[2])
     if (r2_w < cities.Munich[4]) then value =0.998815501 end
   end

   if (si == 19 or si ==2 ) then
     local r2_f = (x-cities.Hannover[1])*(x-cities.Hannover[1])+(y-cities.Hannover[2])*(y-cities.Hannover[2])
     if (r2_f < cities.Hannover[4]) then value =  0.993617907 end
   end
  
   if(si == 10) then
     local r2_w = (x-cities.Berlin[1])*(x-cities.Berlin[1])+(y-cities.Berlin[2])*(y-cities.Berlin[2])
     if (r2_w < cities.Berlin[4]) then value =0.996868438 end
   end 

   if (si == 21 or si ==4 ) then
     local r2_f = (x-cities.Heinsberg[1])*(x-cities.Heinsberg[1])+(y-cities.Heinsberg[2])*(y-cities.Heinsberg[2])
     if (r2_f < cities.Heinsberg[4]) then value =  0.950232029 end
   end
   
   return value
end
 
 
function InitialinSubsetsA(x,y,t,si)
   value = 0.0
    if (si == 16 or si==5) then
        local r2_f = (x-cities.Frankfurt[1])*(x-cities.Frankfurt[1])+(y-cities.Frankfurt[2])*(y-cities.Frankfurt[2])
        if (r2_f < 79.03) then value = 0.00351201236 end
    end
      
    if(si ==17 or si == 5) then 
      local r2_w = (x-cities.Wiesbaden[1])*(x-cities.Wiesbaden[1])+(y-cities.Wiesbaden[2])*(y-cities.Wiesbaden[2])
      if (r2_w < 64.80) then value = 0.003116987 end
    end

    if (si == 18 or si ==7 ) then
        local r2_f = (x-cities.Stuttgart[1])*(x-cities.Stuttgart[1])+(y-cities.Stuttgart[2])*(y-cities.Stuttgart[2])
        if (r2_f < cities.Stuttgart[4]) then value =  0.003298521 end
    end
      
    if(si == 20 or si == 8) then
        local r2_w = (x-cities.Munich[1])*(x-cities.Munich[1])+(y-cities.Munich[2])*(y-cities.Munich[2])
        if (r2_w < cities.Munich[4]) then value =0.001184499 end
    end

    if (si == 19 or si ==2 ) then
        local r2_f = (x-cities.Hannover[1])*(x-cities.Hannover[1])+(y-cities.Hannover[2])*(y-cities.Hannover[2])
        if (r2_f < cities.Hannover[4]) then value =  0.006382093 end
    end
      
    if(si == 10) then
        local r2_w = (x-cities.Berlin[1])*(x-cities.Berlin[1])+(y-cities.Berlin[2])*(y-cities.Berlin[2])
        if (r2_w < cities.Berlin[4]) then value =0.003131562 end
    end 
    
    if (si == 21 or si ==4 ) then
        local r2_f = (x-cities.Heinsberg[1])*(x-cities.Heinsberg[1])+(y-cities.Heinsberg[2])*(y-cities.Heinsberg[2])
        if (r2_f < cities.Heinsberg[4]) then value =  0.049767970 end
    end

   return value
end
 

corona_DE_7Cites = {
  grid = {
    filename = geom_home.."DE-7_cities.ugx",
    mandatory = {"SH","HH","NI","HB",
                 "NW","HE","RP","BW",
                 "BY","SL","BE","BB",
                 "MV","SN","ST","TH", "FRA","WIES","STU","HANN","MUN","HEINS"},
  },
  
  regions = 
  {
     -- Quelle: DESTATIS, 02_bundeslaender.xlsx (31.12.2018)  
    {subset = "SH", area= 15804.30, density =  183, diffusion = MyDefaultDiffusion}, 
    {subset = "HH", area=   755.09, density = 2438, diffusion = MyDefaultDiffusion}, 
    {subset = "NI", area= 47709.50, density =  167, diffusion = MyDefaultDiffusion}, 
    {subset = "HB", area=   419.36, density = 1629, diffusion = MyDefaultDiffusion}, 
    {subset = "NW", area= 34112.31, density =  526, diffusion = MyDefaultDiffusion}, 
    {subset = "HE", area= 21115.68, density =  297, diffusion = MyDefaultDiffusion}, 
    {subset = "RP", area= 19858.00, density =  206, diffusion = MyDefaultDiffusion}, 
    {subset = "BW", area= 35748.22, density =  310, diffusion = MyDefaultDiffusion}, 
    {subset = "BY", area= 70541.57, density =  185, diffusion = MyDefaultDiffusion}, 
    {subset = "SL", area=  2571.11, density =  385, diffusion = MyDefaultDiffusion}, 
    {subset = "BE", area=   891.12, density = 4090, diffusion = MyDefaultDiffusion}, 
    {subset = "BB", area= 29654.48, density =   85, diffusion = MyDefaultDiffusion}, 
    {subset = "MV", area= 23294.62, density =   69, diffusion = MyDefaultDiffusion}, 
    {subset = "SN", area= 18449.96, density =  221, diffusion = MyDefaultDiffusion}, 
    {subset = "ST", area= 20454.31, density =  108, diffusion = MyDefaultDiffusion},
    {subset = "TH", area= 16202.37, density =  132, diffusion = MyDefaultDiffusion},
    {subset = "FRA", area= 248.31, density =  3074, diffusion = MyDefaultDiffusion},
    {subset = "WIES", area= 203.93, density =  1366, diffusion = MyDefaultDiffusion},
    {subset = "STU", area= 207.36, density =  3100, diffusion = MyDefaultDiffusion},
    {subset = "HANN", area= 204.14, density =  2630, diffusion = MyDefaultDiffusion},
    {subset = "MUN", area= 310.7, density =  4800, diffusion = MyDefaultDiffusion},
    {subset = "HEINS", area= 92.14, density =  460, diffusion = MyDefaultDiffusion},
      
  }
 

}

function corona_DE_7Cites.create_domain(self)

    local domain = util.CreateDomain(self.grid.filename, 0) 
    self.myCompositeDensity =  CompositeUserNumber(false) -- discontinuous data
    
    -- initialize density
    for key, value in pairs(self.regions) do
       local si = domain:subset_handler():get_subset_index(value.subset)
       print("Initialize "..key..":"..value.subset.."(si="..si..")")
       self.myCompositeDensity:add(si, ConstUserNumber(value.density))
       
    end

    return domain
end


function corona_DE_7Cites.set_initial_values(self, u)
    u:set(0.0)
    --Interpolate("testG", u, "g")
    --Interpolate("testA", u, "a")

    for key, val in pairs(corona_DE_7Cites.grid.mandatory) do
      Interpolate("InitialinSubsetsG", u, "g",val)
      Interpolate("InitialinSubsetsA", u, "a",val)
    end

end



return corona_DE_7Cites
