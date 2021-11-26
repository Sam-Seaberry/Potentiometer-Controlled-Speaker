#include "mbed.h"
#include "C12832.h"
RawSerial USART(PA_11, PA_12);
C12832 lcd(D11, D13, D12, D7, D10);

class Potentiometer {
    private:
        AnalogIn inputSignal;
        float VDD, currentSamplePC, currentSampleVolts;
    public:
        Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {
            currentSamplePC = 0.0;
            currentSampleVolts = 0.0;
            }
        float amplitudeVolts(void) { return (inputSignal.read()*VDD); }
        float amplitudePC(void) { return inputSignal.read(); }
        
        void sample(void) {
            currentSamplePC = inputSignal.read();
            currentSampleVolts = currentSamplePC*VDD;
        }
        
        float getCurrentSampleVolts(void) { 
            return currentSampleVolts; 
        }
        
        float getCurrentSamplePC(void) {
             return currentSamplePC; 
        }

};

class SamplePot : public Potentiometer{
    private:
        
        float freq;
        
        float cycle;
        Ticker tkr;
    public:
        SamplePot(PinName pin,float v, float f) : Potentiometer(pin,v) {
            freq= f;
            cycle = 1/f;
            
            tkr.attach(callback(this, &SamplePot::sample),cycle);
            }
};

class PwmSpeaker {
    private:
        PwmOut outputSignal;
        float period; // in sec
        float frequency; // in Hz
        float minFreq, maxFreq; // min/max in Hz
        SamplePot* pot;
        
        
    public:
        PwmSpeaker(PinName pin, float freq, float min, float max, SamplePot* p ): outputSignal(pin), frequency(freq), minFreq(min), maxFreq(max), pot(p){
            outputSignal.write(.5);
            }

        void setFreq(float f){
           frequency = f;
          
         }
         void setmin(float min){
             minFreq = min;
             }
        void setmax(float max){
            maxFreq = max;
            }
        
         
        float getFreq(void){
         return frequency;
         }
         void updatefreq(void){
             frequency = pot->getCurrentSampleVolts()*(maxFreq-minFreq)+minFreq;
             period = 1/frequency;
             outputSignal.period(period);
             }
         // other getters
       
};
int main() {
    
    Potentiometer pot(A0, 3.3);
    SamplePot* sample= new  SamplePot(A0,3.3,500);
    PwmSpeaker* speak = new PwmSpeaker(D6,1000,2000,9000, sample);
    
    
    
    
    //float s = 0;
    while (1){
        speak->updatefreq();
        lcd.locate(0,15);
        lcd.printf("Counting: %f",speak->getFreq());
        }
}
