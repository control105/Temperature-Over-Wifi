#define DHTPIN 10    // what digital pin we're connected to
#define DHTTYPE DHT11   // specify the type of DHT sensor


float t, f, h; //For DHT value variables(Temperature in Celsius, temperature in Fahrenheit, Humidity)

String data;

//Create the object of DHT
DHT dht(DHTPIN, DHTTYPE);


void ReadHumTemp() {
  //Read humidity
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);
  Serial.print("Humi: ");
  Serial.println(h);
  Serial.print("Temp C: ");
  Serial.println(t);
  Serial.print("Temp F: ");
  Serial.println(f);
  Serial.print("\n\n");
}



String getSensorsData(){  
  ReadHumTemp();  //Read temperature and humidity.
  data = "Data: *" + String(t) + "Celsius" + "\n" + String(h) + "\n" + String(f) + "Fahrenheit" + "\n" +"#";
  return data; 
}

