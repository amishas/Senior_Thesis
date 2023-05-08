function testDoPost() {
  var e = {};
  e.parameter = {};
  e.parameter.event = 'test';
  e.parameter.data = "{\"thermistor\":10,\"mlx_object\":20,\"mlx_ambient\":30}";
  e.parameter.coreid = '1f0030001647ffffffffffff';
  e.parameter.published_at = new Date().toISOString();
  Logger.log(e);
  doPost(e);
}

function doPost(e) {
  var ss = SpreadsheetApp.getActiveSpreadsheet();
  var publishedAt = new Date(e.parameter.published_at);
  var event = String(e.parameter.event);

  try {
    var json_data = JSON.parse(e.parameter.data);
  } catch(err)  {
    Logger.log("Unable to return JSON: " + err);
    var result = {};
    result.result = "Unable to return JSON: " + err;
    return ContentService.createTextOutput(JSON.stringify(result)).setMimeType(ContentService.MimeType.JSON);
  }
  
  var sheet = ss.getSheetByName(event);
  if (sheet == null) {
    sheet = ss.insertSheet(event, 0,);
    var titleRow = ["Timestamp", "JSON Data"];
    for (key in json_data) {
      titleRow = titleRow.concat(key);
    }
    sheet.appendRow(titleRow);
  }
  sheet.activate();
  Logger.log("Sheet: " + sheet.getSheetName());

  if (sheet == null) {
    Logger.log("Unable to find sheet: " + err);
    var result = {};
    result.result = "Unable to find sheet: " + err;
    return ContentService.createTextOutput(JSON.stringify(result)).setMimeType(ContentService.MimeType.JSON);
  }

  try {
    var row = [].concat(publishedAt); // Timestamp
    row = row.concat(json_data); // Raw JSON data
    for (key in json_data) {
      row = row.concat(json_data[key]);
    }
    Logger.log("Row data: " + row);

  } catch(err) {
    Logger.log("Unable to log data: " + err);
    var result = {};
    result.result = "Unable to log data: " + err;
    return ContentService.createTextOutput(JSON.stringify(result)).setMimeType(ContentService.MimeType.JSON);
  }

  sheet.appendRow(row);
  var result = {};
  result.result = "success";
  return ContentService.createTextOutput(JSON.stringify(result)).setMimeType(ContentService.MimeType.JSON);
}

// Use timed trigger to log weather data
function getWeather() {
  // Variables accessed from OpenWeather
  var main_keys = ["main", "temp", "feels_like", "humidity", "temp_min", "temp_max"];
  var weather_keys = ["weather", "main", "description"];
  var wind_keys = ["wind", "speed"];
  var key_list = [main_keys, weather_keys, wind_keys];
  
  // Get Sheet
  var sheetName = "Current Weather";
  var ss = SpreadsheetApp.getActiveSpreadsheet();
  var sheet = ss.getSheetByName(sheetName);
  if (sheet == null) {
    sheet = ss.insertSheet(sheetName, 0,);
    var titleRow = ["Timestamp", "API Timestamp", "JSON Data"];
    for (let i = 0; i < key_list.length; i++) {
      for (let j = 1; j < key_list[i].length; j++) {
        titleRow = titleRow.concat(key_list[i][j]);
      } 
    }
    sheet.appendRow(titleRow);
  }

  // Weather Data from OpenWeatherAPI
  var time = new Date();
  var response = UrlFetchApp.fetch("https://api.openweathermap.org/data/2.5/weather?id=5102922&appid=e0b0a842bc0fa80431bf385103d2e92f&units=metric")
  // change appid to current API key
  try {
    var json = JSON.parse(response.getContentText()); // parse the JSON
  } catch (e) {
    Logger.log("Unable to return JSON");
  }

  // Weather Data Log
  var calculatedAt = new Date(json.dt * 1000);
  
  var row = [].concat([time, calculatedAt]);
  row = row.concat(json); // Raw JSON data

  for (let i = 1; i < main_keys.length; i++) {
    row = row.concat(json[main_keys[0]][main_keys[i]]);
  }
  for (let i = 1; i < weather_keys.length; i++) {
    row = row.concat(json[weather_keys[0]][0][weather_keys[i]]);
  }
  for (let i = 1; i < wind_keys.length; i++) {
    row = row.concat(json[wind_keys[0]][wind_keys[i]]);
  }
  Logger.log("Row data: " + row);
  sheet.appendRow(row);
}

function doGet() {
  return HtmlService.createHtmlOutputFromFile('Index');
}
