import requests, json
import numpy as np
import datetime
from datetime import datetime, timedelta

class RKIDataLoader:
	
	def __init__(self):
		self.saved_days=100 #number of days to save in the tables
		self.current_date=-1 #date of the most recent datapoint of the last dataset that was loaded
		self.load_area_designations()
		return
		
	def get_area_name(self,id):
		for i in range(len(self.area_names)):
			if int(self.area_names[i]["attributes"]['AdmUnitId'])==id:
				return self.area_names[i]["attributes"]['Name']
		return ""
				
	def search_previous_data(self,table,date):
		current_min_dist=99999999999
		ci=-1
		d1=datetime.strptime(datetime.utcfromtimestamp((date/1000)).strftime('%Y-%m-%d'),'%Y-%m-%d').date()
		best_date=table[0]['attributes']['Datum']	
		for i in range(len(table)):
			current_date=table[i]['attributes']['Datum']				
	
			d2=datetime.strptime(datetime.utcfromtimestamp((current_date/1000)).strftime('%Y-%m-%d'),'%Y-%m-%d').date()	
			if (d1-d2).days < 4 and (d1-d2).days > 0:
				ci=i
				best_date=current_date
				
		return (best_date,ci)
		
	def load_area_designations(self):
		url = "https://services7.arcgis.com/mOBPykOjAyBO2ZKk/arcgis/rest/services/rki_admunit_v/FeatureServer/0/query?"
		parameter = {
			'referer':'https://www.mywebapp.com',
			'user-agent':'python-requests/2.9.1',
			'where': '1=1', # Welche landkreise sollen zurueckgegeben werden
			'outFields': '*', # Rueckgabe aller Felder
			'returnGeometry': False, # Keine Geometrien
			'f':'json', # Rueckgabeformat, hier JSON
			'cacheHint': True # Zugriff ueber CDN anfragen
		}
		result = requests.get(url=url, params=parameter) #Anfrage absetzen
		resultjson = json.loads(result.text) # Das Ergebnis JSON als Python Dictionary laden
		self.area_names=resultjson['features']
		#print(resultjson['features'][0])		
			
	def get_landkreis_data(self,lk_id):
		url = "https://services7.arcgis.com/mOBPykOjAyBO2ZKk/arcgis/rest/services/rki_history_v/FeatureServer/0/query?"
		parameter = {
			'referer':'https://www.mywebapp.com',
			'user-agent':'python-requests/2.9.1',
			'where': f'AdmUnitId = {lk_id}', # Welche landkreise sollen zurueckgegeben werden
			'outFields': '*', # Rueckgabe aller Felder
			'returnGeometry': False, # Keine Geometrien
			'f':'json', # Rueckgabeformat, hier JSON
			'cacheHint': True # Zugriff ueber CDN anfragen
		}
		result = requests.get(url=url, params=parameter) #Anfrage absetzen
		resultjson = json.loads(result.text) # Das Ergebnis JSON als Python Dictionary laden
		len_days=len(resultjson['features'])
		#print(len(resultjson['features'])) # Wir erwarten genau einen Datensatz, Ausgabe aller Attribute
		output=np.zeros(shape=(self.saved_days,2))
		
		cmax=0
		ci=0

		for i in range(len_days):
			if resultjson['features'][i]['attributes']['Datum']>cmax:
				cmax=resultjson['features'][i]['attributes']['Datum']
				ci=i
				
		current_date=0
		previous_date=0	
		#print(resultjson['features'])
		schranke=False
		for i in range(self.saved_days):
			current_date=resultjson['features'][ci-i]['attributes']['Datum']
				
			if i == 0:
				output[self.saved_days-1-i,0]=self.saved_days-1
				self.current_date=datetime.utcfromtimestamp(current_date/1000).strftime('%Y-%m-%d %H:%M:%S')				
			else:
				if current_date>previous_date:
					
					(new_date,new_index)=self.search_previous_data(resultjson['features'],previous_date)
					if new_index<0:
						print("Fehler: Der RKI Datensatz ist nicht streng monoton steigend. Fehler in Iteration "+str(i))
						return ([],False)	
					else:
						current_date=new_date	
						output[self.saved_days-1-i,1]=resultjson['features'][new_index]['attributes']['AnzFallMeldung']
						schranke=True
							
				ts = current_date/1000 
				d1=datetime.strptime(datetime.utcfromtimestamp((current_date/1000)).strftime('%Y-%m-%d'),'%Y-%m-%d').date()
				d2=datetime.strptime(datetime.utcfromtimestamp((previous_date/1000)).strftime('%Y-%m-%d'),'%Y-%m-%d').date()	
				if (d2-d1).days > 4:
					(new_date,new_index)=self.search_previous_data(resultjson['features'],previous_date)
					if new_index<0:
						print("Fehler: Der Abstand zwischen den Tagen im Datensatz beträgt teilweise " +str((d2-d1).days) +" Tage. Das ist zu viel")
						return ([],False)	
					else:
						current_date=new_date	
						output[self.saved_days-1-i,1]=resultjson['features'][new_index]['attributes']['AnzFallMeldung']
						ts = current_date/1000 
						d1=datetime.strptime(datetime.utcfromtimestamp((current_date/1000)).strftime('%Y-%m-%d'),'%Y-%m-%d').date()
						d2=datetime.strptime(datetime.utcfromtimestamp((previous_date/1000)).strftime('%Y-%m-%d'),'%Y-%m-%d').date()						
						schranke=True
				output[self.saved_days-1-i,0]=output[self.saved_days-i,0]-(d2-d1).days

			# if you encounter a "year is out of range" error the timestamp
			# may be in milliseconds, try `ts /= 1000` in that case
			#print(datetime.utcfromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S'))			     
			previous_date=current_date
			if schranke==False:
				output[self.saved_days-1-i,1]=resultjson['features'][ci-i]['attributes']['AnzFallMeldung']
			else:
				schranke=False
		return (output,True)
			
		
	def check_status(self):
		url = "https://services7.arcgis.com/mOBPykOjAyBO2ZKk/arcgis/rest/services/rki_service_status_v/FeatureServer/0/query?"
		parameter = {
		'referer':'https://www.mywebapp.com',
			'user-agent':'python-requests/2.9.1',
			'where': '1=1', # Alle Status-Datensätze
			'outFields': '*', # Rückgabe aller Felder
			'returnGeometry': False, # Keine Geometrien
			'f':'json', # Rückgabeformat, hier JSON
		'cacheHint': True # Zugriff über CDN anfragen
		}
		result = requests.get(url=url, params=parameter) #Anfrage absetzen
		resultjson = json.loads(result.text) # Das Ergebnis JSON als Python Dictionary laden
		for i in range(len(resultjson['features'])):
			if (resultjson['features'][i]['attributes']['Status'] != 'OK'):
				print(resultjson['features'][i]['attributes']['Status'])
				print("RKI Service status nicht OK für ID "+str(i))
				return False
		return True
	
def save_all_data_to_file(path):	
	dataloader=RKIDataLoader()	
	server_status=dataloader.check_status()
	if (True):
		#Land und Bundesländer
		for i in range(0,17):
			(output,noerror)=dataloader.get_landkreis_data(i) #Gebiets ID. Z.B. 2 steht für Hamburg
			if noerror:
				np.savetxt(path+dataloader.get_area_name(i)+".csv",output, delimiter=",", header="Zeit (Tage),Gemeldete Fallzahlen", fmt='%f')
				print(output)
				print(dataloader.get_area_name(i)+": Datum des letzten Datenpunktes:" ,dataloader.current_date)
			else:
				print("Laden der Daten von "+ dataloader.get_area_name(i)+" war nicht erfolgreich")
				
		#Landkreise
		for i in range(1000,16077):
			if dataloader.get_area_name(i) != "":
				(output,noerror)=dataloader.get_landkreis_data(i) 
				if noerror:
					np.savetxt(path+dataloader.get_area_name(i)+".csv",output, delimiter=",", header="Zeit (Tage),Gemeldete Fallzahlen",fmt='%f')
					print(output)
					print(dataloader.get_area_name(i)+": Datum des letzten Datenpunktes:" ,dataloader.current_date)
				else:
					print("Laden der Daten von "+ dataloader.get_area_name(i)+" war nicht erfolgreich")

path="/home/devanshr/Programs/ug4/plugins/ConstrainedOptimization/examples/SEIRD_model/expData/datensammel_scripts/saved_data/"
save_all_data_to_file(path)
print("Programm beendet")

def load_area_names():
	names=""
	dataloader=RKIDataLoader()	
	#Land und Bundesländer
	for i in range(0,17):
		names+='"'+dataloader.get_area_name(i)+'",'
		print(i)

			
	#Landkreise
	for i in range(1000,16077):
		if dataloader.get_area_name(i) != "":
			names+='"'+dataloader.get_area_name(i)+'",'
				
	with open("areanames2.txt", "w") as text_file:
		text_file.write(names)
    
#load_area_names()
