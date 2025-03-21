#include <EasyNextionLibrary.h>
#include <trigger.h>
      EasyNex myNex(Serial); 
      #define LOOP_TIME 1000

        unsigned long timer ;
       unsigned long pageRefreshTimer = millis(); // Timer for DATA_REFRESH_RATE
        #define DATA_REFRESH_RATE 1000 

        String text1,text2 ;
        int Number_read ;
        int Number_Write ;
      
        bool newPageLoaded = false; 
  
    void setup() {
       
        myNex.begin(9600);
                            
          
          
        delay(500);              
        myNex.writeStr("page 0"); 
        delay(50);
        myNex.lastCurrentPageId = 1; 
                                      
    }

    void loop() {
   
        myNex.NextionListen(); 
                                
 
        if ((millis() - timer) > LOOP_TIME) {
          
            refereshCurrentPage();
            firstRefresh();
         
            timer = millis();
        } else {
            
        }
    }

    void firstRefresh() {
  

        
          if(myNex.currentPageId != myNex.lastCurrentPageId){ 
            
            newPageLoaded = true;               
            switch(myNex.currentPageId){
              case 0:
                refreshPage0();
                break;
              
              case 1:
                refreshPage1();
                break;
                
              case 2:
                refreshPage2();
                break;
        
              case 3:
                refreshPage3();
                break;
              case 4:
                refreshPage4();
                break;
                
           
            }
            
            newPageLoaded = false; 
            
            myNex.lastCurrentPageId = myNex.currentPageId;
          }
    }

    void refereshCurrentPage() {
     
        if ((millis() - pageRefreshTimer) > DATA_REFRESH_RATE) {
         
              
                switch(myNex.currentPageId){
                  case 0:
                    refreshPage0();
                    break;
                  
                  case 1:
                    refreshPage1();
                    break;
                    
                  case 2:
                    refreshPage2();
                    break;
            
                  case 3:
                    refreshPage3();
                    break;
                    
                  case 4:
                    refreshPage4();
                    break;
                    
                 
                 
                }
                pageRefreshTimer = millis();
        } else {
            
        }
    }

    void refreshPage0() {
        
        text1 = myNex.readStr("t1.txt") ;
        text1 = myNex.readStr("t1.txt") ;
                
        text2 = myNex.readStr("t2.txt") ;
        text2 = myNex.readStr("t2.txt") ;
            
         
        myNex.writeStr("t3.txt","mecha") ;
        myNex.writeStr("t3.txt","mecha") ;
                
        myNex.writeStr("t4.txt","Eng") ;
        myNex.writeStr("t4.txt","Eng") ;
      
        Serial.println("") ;

        Serial.print("text 1 is "   ) ;
        Serial.println(text1) ;
        Serial.print("text 2 is "   ) ;
        Serial.println(text2) ;
        Serial.println("page 0") ;
    }

    void refreshPage1() {
      
        Number_read = myNex.readNumber("n0.val");
        Number_read = myNex.readNumber("n0.val");
       
        Serial.print("the number is: ");
        Serial.println(Number_read);
        Serial.println("page 1");
    }

    void refreshPage2() {
       
        myNex.writeNum("n0.val",666);
        myNex.writeNum("n0.val",666);
        Serial.println("page 2") ;
    }

    void refreshPage3() {
        
    }

    void refreshPage4() {
        
    }
