// LCD module connections
sbit LCD_RS at RC0_bit;
sbit LCD_EN at RC1_bit;
sbit LCD_D4 at RC6_bit;
sbit LCD_D5 at RC3_bit;
sbit LCD_D6 at RC4_bit;
sbit LCD_D7 at RC5_bit;

sbit LCD_RS_Direction at TRISC0_bit;
sbit LCD_EN_Direction at TRISC1_bit;
sbit LCD_D4_Direction at TRISC6_bit;
sbit LCD_D5_Direction at TRISC3_bit;
sbit LCD_D6_Direction at TRISC4_bit;
sbit LCD_D7_Direction at TRISC5_bit;
// End LCD module connections

sbit m11 at portb.b3;
sbit m12 at portb.b4;

signed int  pulsos=0;  //variable del encoder
char texto[25];
unsigned int codigo;   //variables pid
float kp=10,ti=500,td=60;
float error,up,ui=0.0,ui_=0.0,error_=0.0,ud,ut=0.0,d_error=0;
float ki,kd;
float setpoint=0;
void pid(float pid_in){

const float max_x=255.0;   //limites del pwm
const float min_x=-255.0;   //limites del pwm
const float t_muestreo=1.0;//4e-3
//Calculando el error
error=setpoint-pid_in;
   up=kp*error;
       if (ti!=0) ki=(kp/ti);
       else ki=0;
       ui=ui_ + (ki*t_muestreo*error);
       kd=kp*td;
       d_error=error-error_;
       ud=kd*(d_error)/t_muestreo;
        ut=up+ui+ud;
        if (ut>max_x) ut=max_x;
        if (ut<min_x) ut=min_x;
        ui_=ui;
        error_=error;

}







void Interrpciones() iv 0x0004 ics ICS_AUTO
 {
   if (INTF_bit==1)
   {
   if (PORTB.RB1==PORTB.RB0) pulsos++; else pulsos--;
   if (pulsos>50) PULSOS=50;
   else if  (pulsos<-50) PULSOS=-50;
   INTEDG_bit=~INTEDG_bit; //cambia el flanco a detectar modo 2x
   INTF_bit=0; //limpia bandera de interrupcion
   }
 
}

void main()
 {
 TRISB=255;  //TODO ENTRADAS
 TRISC=0;    //TODOS SALIDAS
 trisa=255; //todo entradas
 
 trisb.b3=0;   //salida control de giro
 trisb.b4=0;  //Salida Contro de giro
 
 //Todo Analogo
adcon0.b0=0;
adcon1=0;
Lcd_Init();                        // Initialize LCD

  Lcd_Cmd(_LCD_CLEAR);               // Clear display
  Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
  Lcd_Out(1,1,"    CONTROL ARZ");
  Lcd_Out(2,1,"SET_POINT=");
  Lcd_Out(4,1,"PULSOS=");
  
  ADC_Init();
  PWM1_Init(400);
  PWM1_Set_Duty(0);
  PWM1_Start();
  
 // UART1_Init(9600);
  
  //CONFIGURACION INTERRPCIONES
  INTCON=0B01000000;
  
  INTF_bit=0; //limpia la bansera e interrupcion
  INTEDG_bit=0; //por flanco de bajada
  INTE_bit=1; //habilita interrpcion por flanco
  GIE_bit=1; //Autoriza interrupciones
  
  
  


while (1)
{
  codigo=ADC_Get_Sample(0);
  //los pulsos en proteus van de -50 a 50  debe colocarse el set point en ese rango
  //equivale aprox -180 a 180 grados
  setpoint=  0.0978*codigo-50.0;

     pid(pulsos);

if (ut<0)    //gira a la derecha     analiza la salida para determinar giro
  {
   ut=-ut;   // el valor del pwm en cada sentido debe ser solo positivo
   m11=1;
   m12=0;
  }
  else       //gira a la izquierda
  {
    m11=0;
    m12=1;

  }



   PWM1_Set_Duty(ut); //salida pid   po PWM

   inttostr(setpoint,texto);
 
      lcd_Out(2,11,"       ");
      Lcd_Out(2,11,texto);

    inttostr(pulsos,texto);
   lcd_Out(4,8,"      ");
   lcd_Out(4,8,texto);


// Delay_ms(20);

}



}