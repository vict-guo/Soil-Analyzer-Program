import controlP5.*;
import processing.serial.*;
import processing.video.*;
import javax.swing.*;
import java.util.regex.Pattern;
ControlP5 cp51;
ControlP5 cp52;
ControlP5 cp53;

Serial port; //bluetooth

Capture video;

Textlabel title, prompt1,prompt2,lib,status;

Textarea data;

PImage bg, display,leftArrow,rightArrow;

int cnt = 0,idx = 0, num = 0;

boolean stream = false, library = false;

ArrayList<String> pics = new ArrayList<String>();

String[] tokens = new String[9]; //tokens of strings
String string;

long lastDebounceTime, debounce = 50;
boolean isPressed = false, done = false;

void reset(){
   for(int i = 0; i < 9; i++){
      tokens[i] = "?"; 
   }
}

void setup() {
  size(1000,850);
  cp51 = new ControlP5(this);
  cp52 = new ControlP5(this);
  cp53 = new ControlP5(this);
  noStroke();
  
  //bluetooth
  port = new Serial(this,"COM16",9600);
  port.bufferUntil('\n');
  
  video = new Capture(this,"name=Microsoft® LifeCam VX-2000,size=640x480,fps=30");
  bg = loadImage("background.jpg");
  bg.resize(1000,850);
  leftArrow = loadImage("left.png");
  leftArrow.resize(50,40);
  rightArrow = loadImage("right.png");
  rightArrow.resize(50,40);
  reset();
  menu();

}

void draw() {
  if(stream){
    video.read();
    image(video,25,100,665,580);
    if (mousePressed && mouseX <= 685 && mouseX >= 25 && mouseY <=680 && mouseY >= 100){
      if(!isPressed){
         isPressed = true;
         lastDebounceTime = millis();     
      }
      if(millis() - lastDebounceTime > debounce && !done){
        print(mouseX);
        print(" ,");
        println(mouseY);
        done = true;
        reset();
        javax.swing.JOptionPane.showMessageDialog(null, "Location selection successful!", "Remote Soil Analyzer",JOptionPane.INFORMATION_MESSAGE);
        
        //bluetooth
        port.write(str(mouseX)+"\n");
        delay(50);
        port.write(str(mouseY)+"\n");
      }
    }
    else{
      isPressed = false; 
    }
    if(!done){
       fill(#53f441);
       ellipse(710,64,25,25);
       status.setText(""); 
    }
    else{
       fill(#f4eb42);
       ellipse(710,64,25,25);
       status.setText("Analysis in Progress..."); 
    }
  
   if(!tokens[7].isEmpty() && !tokens[7].equals("?")){
       data.setText("X coord of Sample location: "+tokens[0]+" cm \n"
                    +"Y coord of Sample location: "+tokens[1]+" cm \n"
                    +"Mass of Sample: "+tokens[2]+" g \n"
                    +"Volume of Sample: "+tokens[3]+" cm3 \n"
                    +"Temperature: "+tokens[4]+" C \n"
                    +"R : "+tokens[5]+"\n"
                    +"G : "+tokens[6]+"\n"
                    +"B : 144\n"
                    +"Battery Voltage: "+tokens[8]+"\n"
                    );   
        delay(5000);
       done = false;
       pics.add("/Saved Pictures/screen-"+num+".png");
      saveFrame("/Saved Pictures/screen-"+num+".png");
      num++;
      // reset();
   }
   else{
       data.setText("X coord of Sample location: "+tokens[0]+" cm \n"
                    +"Y coord of Sample location: "+tokens[1]+" cm \n"
                    +"Mass of Sample: "+tokens[2]+" g \n"
                    +"Volume of Sample: "+tokens[3]+" cm3 \n"
                    +"Temperature: "+tokens[4]+" C \n"
                    +"R : "+tokens[5]+"\n"
                    +"G : "+tokens[6]+"\n"
                    +"B : "+tokens[7]+"\n"
                    +"Battery Voltage: "+tokens[8]+"\n"
                    );   
                    
  }
  }
  else if(library){
     if(pics.size() > 0){
         if(idx < 0)
           idx = 0;
         else if(idx >= pics.size())
           idx = pics.size() - 1;
         display = loadImage(pics.get(idx));
         display.resize(750,600);
         image(display,125,75);
     }
  }
}

public void controlEvent(ControlEvent theEvent) {
  if(theEvent.getController().getName().equals("Start") && cnt > 1){
      cameraStart();
  }
  else if(theEvent.getController().getName().equals("Quit") && cnt > 1){
      exit();
  }
  else if(theEvent.getController().getName().equals("Snap Photo")){
    pics.add("/Saved Pictures/screen-"+num+".png");
    saveFrame("/Saved Pictures/screen-"+num+".png");
    num++;
    javax.swing.JOptionPane.showMessageDialog(null, "Photo Taken!", "Remote Soil Analyzer",JOptionPane.INFORMATION_MESSAGE);
  }
  else if(theEvent.getController().getName().equals("View Saved Photos")){
    removeCameraStart();
    ImageLibrary();
  }
  else if(theEvent.getController().getName().equals("Back")){
    removeCameraStart();
    menu();
  }
  else if(theEvent.getController().getName().equals("Right")){
    idx++;
  }
  else if(theEvent.getController().getName().equals("Left")){
    idx--;
  }
  else if(theEvent.getController().getName().equals("Return")){
      cameraStart();
  }
  if(!stream && !library)
    cnt++;
  else
    cnt = 0;
  println(theEvent.getController().getName());
}

public void menu(){
  library = false;
  stream = false;
  cp51.show();
  cp52.hide();
  cp53.hide();
     // create a new button with name 'Start'
  cp51.addButton("Start")
     .setValue(0)
     .setPosition(450,650)
     .setSize(100,60)
     .setFont(createFont("Georgia", 20))
     ;
   
   cp51.addButton("Quit")
     .setValue(0)
     .setPosition(450,750)
     .setSize(100,60)
     .setFont(createFont("Georgia", 20))
     ;
     
   title = cp51.addTextlabel("title")
               .setText("Remote Soil Analyzer")
               .setPosition(200,100)
               .setFont(createFont("Georgia", 65))
               ;
   background(bg);
}  

public void cameraStart(){
  video.start();
  stream = true;
  library = false;
  
  background(255,255,255);
  
  cp52.show();
  cp51.hide();
  cp53.hide();
  
  prompt1 = cp52.addTextlabel("prompt1")
               .setText("Select location to start collection.")
               .setColorValue(0)
               .setPosition(25,50)
               .setFont(createFont("Georgia", 20))
               ;
  prompt2 = cp52.addTextlabel("prompt2")
               .setText("Data Received:")
               .setColorValue(0)
               .setPosition(750,50)
               .setFont(createFont("Georgia", 20))
               ;
  status = cp52.addTextlabel("status")
               .setColorValue(0)
               .setPosition(750,85)
               .setFont(createFont("Georgia", 20))
               ;
  data = cp52.addTextarea("txt")
                  .setPosition(725,125)
                  .setSize(240,600)
                  .setFont(createFont("georgia",14))
                  .setLineHeight(16)
                  .setColor(color(0))
                  .setColorBackground(color(255,100))
                  .setColorForeground(color(255,100));
                  ;           
  cp52.addButton("Snap Photo")
     .setPosition(125,750)
     .setSize(100,60)
     .setFont(createFont("Georgia", 14))
     ;
  cp52.addButton("View Saved Photos")
     .setPosition(325,750)
     .setSize(150,60)
     .setFont(createFont("Georgia", 14))
     ;
  cp52.addButton("Back")
     .setPosition(525,750)
     .setSize(100,60)
     .setFont(createFont("Georgia", 14))
     ;
}  

public void removeCameraStart(){
  video.stop();
  stream = false;
  library = true;
}

public void ImageLibrary(){
   cp53.show();
   cp51.hide();
   cp52.hide();
   background(0);
   lib = cp53.addTextlabel("lib")
               .setText("Saved Images")
               .setPosition(125,25)
               .setFont(createFont("Georgia", 30))
               ;
   cp53.addButton("Right")
     .setPosition(900,400)
     .setImage(rightArrow)
     .setSize(50,40)
     .setFont(createFont("Georgia", 14))
     ;
   cp53.addButton("Left")
     .setPosition(50,400)
     .setImage(leftArrow)
     .setSize(50,40)
     .setFont(createFont("Georgia", 14))
     ;
   cp53.addButton("Return")
     .setPosition(450,750)
     .setSize(100,60)
     .setFont(createFont("Georgia", 14))
     ;
   
}

void serialEvent( Serial port) {
//put the incoming data into a String - 
//the '\n' is our end delimiter indicating the end of a complete packet
  string = port.readStringUntil('\n');
//make sure our data isn't empty before continuing
  if (string != null) {
  //trim whitespace and formatting characters (like carriage return)
    string = trim(string);
    tokens = string.split(Pattern.quote("--")); 
    println(string);
  }
  port.clear();
}
