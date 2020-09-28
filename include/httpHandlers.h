

/*__________________________________________________________SERVER_HANDLERS__________________________________________________________*/

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (LittleFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = LittleFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

void handleFileUpload(){ // upload a new file to the LittleFS
  HTTPUpload& upload = server.upload();
  String path;
  if(upload.status == UPLOAD_FILE_START){
    path = upload.filename;
    if(!path.startsWith("/")) path = "/"+path;
    if(!path.endsWith(".gz")) {                          // The file server always prefers a compressed version of a file 
      String pathWithGz = path+".gz";                    // So if an uploaded file is not compressed, the existing compressed
      if(LittleFS.exists(pathWithGz))                      // version of that file must be deleted (if it exists)
         LittleFS.remove(pathWithGz);
    }
    Serial.print("handleFileUpload Name: "); Serial.println(path);
    fsUploadFile = LittleFS.open(path, "w");            // Open the file for writing in LittleFS (create if it doesn't exist)
    path = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location","/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

int updateFile(String text) {

  text.replace("description=","");

  int bytesWritten = 0;
  File file = LittleFS.open("/gradient.json", "w");
  if (!file) {
    Serial.println("Error opening file for writing");
    return bytesWritten;
  }

  bytesWritten = file.print(text);

  if (bytesWritten > 0) {
    Serial.println("File was written");
    Serial.println(bytesWritten);

  } else {
    Serial.println("File write failed");
  }

  file.close();
  return bytesWritten;
}

void handleUpdate() { //Handler for the body path
 
      if (server.hasArg("plain") == false) { //Check if body received
        // server.send(200, "text/plain", "Body not received");
        server.send(500, "text/plain", "500: could not save settings");
        return;
      }

      if (updateFile(server.arg("plain"))) {
        server.sendHeader("Location","/success.html");      // Redirect the client to the success page
        server.send(303);
      } else {
        server.send(500, "text/plain", "500: could not save settings");
      }
 
      
      // String message = "Body received:\n";
      //        message += server.arg("plain");
      //        message += "\n";
 
      // server.send(200, "text/plain", message);
      // Serial.println(message);

}


void handleRoot() {
  
}


void handleNotFound() { // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(server.uri())){          // check if the file exists in the flash memory (LittleFS), if so, send it
    //TODO: Redirect to a 404.html
    server.send(404, "text/plain", "404: File Not Found");
  }
}

