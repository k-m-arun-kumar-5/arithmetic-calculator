unsigned short kp;
char keypad_code[16] = {'7', '8', '9', '/', '4', '5', '6', '*', '1', '2', '3', '-', 'C' ,'0', '=', '+'};

// Keypad module connections
char  keypadPort at PORTD;
// End Keypad module connections

// LCD module connections
sbit LCD_RS at RC4_bit;
sbit LCD_EN at RC5_bit;
sbit LCD_D4 at RC0_bit;
sbit LCD_D5 at RC1_bit;
sbit LCD_D6 at RC2_bit;
sbit LCD_D7 at RC3_bit;

sbit LCD_RS_Direction at TRISC4_bit;
sbit LCD_EN_Direction at TRISC5_bit;
sbit LCD_D4_Direction at TRISC0_bit;
sbit LCD_D5_Direction  at TRISC1_bit;
sbit LCD_D6_Direction at TRISC2_bit;
sbit LCD_D7_Direction at TRISC3_bit;
// End LCD module connections

typedef struct
{
	unsigned int error_flag: 1;
	unsigned int oper_1_minus_flag: 1;
	unsigned int oper_2_minus_flag: 1;
	unsigned int operand_1_flag   : 1;
    unsigned int operand_2_flag   : 1;    
	unsigned int first_char_flag : 1; 
    unsigned int result_flag     : 1;
    unsigned int                 : 1;	
} status_t;

void Calc_Reset();
status_t calc_status;
long int result = 0, operand_1 = 0 , operand_2 = 0;
unsigned char cur_oper = '\0', prev_oper = '\0', oper = '\0', result_str[7], input_num_chars = 0;
void main()
{
        unsigned char Key;
        char *str;        
        
    Keypad_Init();                           // Initialize Keypad
    Lcd_Init();                              // Initialize LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);         // Clear display
        Lcd_Out(1,1, "CALCULATOR");
        Delay_ms(1000);
     Calc_Reset();   
		
    while(1)
    {
          Key = Keypad_Key_Press();
          if(Key)
          {
               Delay_ms(300);
               Key = keypad_code[Key - 1];
			   result_str[input_num_chars] = Key;
			   ++input_num_chars;
               if((calc_status.error_flag || calc_status.result_flag) && Key != 'C')
			   {
				   continue;
			   }                  				   
			   switch(Key)
                       {
                              case '+':							    
							  case '-':							     
                              case '/':
                              case '*':
							     if(result_str[0] == '*' || result_str[0] == '/' || (result_str[0] == '+' && (result_str[1] == '+' || result_str[1] == '-')) || 
								   (result_str[0] == '-' && (result_str[1] == '+' || result_str[1] == '-')))
								 {
										Lcd_Out(2,1, "ERR: oper");
									    Lcd_Cmd(_LCD_CURSOR_OFF);
                                        calc_status.error_flag = 1; 
									    break;
	    						 }
								 if(cur_oper == '\0')
							     {
									    if(calc_status.first_char_flag == 1)
									    {
											if(Key == '-')
											{									
                                                if(!calc_status.oper_1_minus_flag )
					                                calc_status.oper_1_minus_flag = 1;
											    calc_status.first_char_flag = 0;
											    Lcd_Chr_Cp(Key); 
											    break;
											}
											if(Key == '+')
											{
												 calc_status.first_char_flag = 0;
												 Lcd_Chr_Cp(Key); 
											     break;
											}
										}
									cur_oper = Key;
									prev_oper = '\0'; 
                                    calc_status.operand_1_flag = 0;
	                                calc_status.operand_2_flag = 1;									
								}
								else								
								{
									calc_status.first_char_flag = 0;
									prev_oper = cur_oper;
								    cur_oper = Key;
									if(result_str[0] == prev_oper && result_str[1] == cur_oper)
									{
										Lcd_Out(2,1, "ERR: oper");
										Lcd_Cmd(_LCD_CURSOR_OFF);
                                        calc_status.error_flag = 1; 
										break;
									}
									if(calc_status.operand_2_flag == 1)
									{
										if(cur_oper != '-')
										{
											   Lcd_Out(2,1, "ERR: oper");
											   Lcd_Cmd(_LCD_CURSOR_OFF);
                                               calc_status.error_flag = 1; 
											   break;
										}
                                        else
									    {
									      if(!calc_status.oper_2_minus_flag )
					                         calc_status.oper_2_minus_flag = 1; 
									    }										
                                        if(prev_oper == '-')
									   {
										   calc_status.oper_2_minus_flag = 0; 
									       prev_oper = '+';
									   }									   										  
									}									
								}
								if(!calc_status.error_flag)
                                     Lcd_Chr_Cp(Key); 
                          break;
                  case 'C':
                  Calc_Reset();  
                             break;
                 case '=':
				     Lcd_Cmd(_LCD_CURSOR_OFF);
					calc_status.result_flag = 1;
					if(calc_status.operand_2_flag)
					{
                                     Lcd_Chr_Cp(Key);
                   
					if(prev_oper == '\0')
                       oper = cur_oper;
				   else 
					   oper = prev_oper; 
                    				   
                                        calc_status.error_flag = 0;
										if(calc_status.oper_1_minus_flag)
											operand_1 = -operand_1;
                                        if(calc_status.oper_2_minus_flag)
											operand_2 = -operand_2;  										
                    switch(oper)
                                        {
                                            case '+':
                                               result = operand_1 + operand_2;
                                            break;
                        case '-':
                                               result = operand_1 - operand_2;
                                            break;
                        case '*':
                                              result = operand_1 * operand_2;
                                            break;
                             case '/':
                                              if(operand_2)
											  {	  
											       result = operand_1 / operand_2;
											  }
						                      else
                                              {  
                                                  calc_status.error_flag = 1;
                                                      Lcd_Out(2,1, "ERR: div oper2 = 0"); 
													  Lcd_Cmd(_LCD_CURSOR_OFF);
                                                  }
                                           break;
                                           default:
                                                Lcd_Out(2,1, "ERR: Oper");
									   			Lcd_Cmd(_LCD_CURSOR_OFF);
									            calc_status.error_flag = 1;											   
                                    }
									if(!calc_status.error_flag && result <= 32767 && result >= -32768)
									{
									    IntToStr(result, result_str);
                                        str = ltrim(result_str);
									    Lcd_Out_Cp(str);
										if(oper == '/')
										{
											result = operand_1 % operand_2;	
											Lcd_Out(1, 10, "Rem :");	
											IntToStr(result, result_str);
                                            str = ltrim(result_str);
									        Lcd_Out_Cp(str);
										}
									}
                                        
					}						
                break;        
                            default:
                               kp = Key - 0x30;
							    calc_status.first_char_flag = 0;
                               if(calc_status.operand_1_flag == 1)
							   {
								   if(operand_1 > 32767 || operand_1 < -32768)
								   {
									   Lcd_Out(2,1, "ERR: Oper_1"); 
									   				     Lcd_Cmd(_LCD_CURSOR_OFF);
									   calc_status.error_flag = 1;	
								   }
								   else
								   {
                                       operand_1 = (10 * operand_1) + kp;
									   Lcd_Chr_Cp(Key);                         
								   }
							   }
                               else  
							   {
								   if(operand_2 > 32767 || operand_2 < -32768)
								   {
									   Lcd_Out(2,1, "ERR: Oper_2");
									   				     Lcd_Cmd(_LCD_CURSOR_OFF);
									   calc_status.error_flag = 1;	
								   }
								   else
								   {
                                        operand_2  = (10 * operand_2) + kp;
										Lcd_Chr_Cp(Key);                         
								   }
							   }								   

                    }
              }
        }
}

void Calc_Reset()
{
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_BLINK_CURSOR_ON);            // Cursor oN
  operand_1 = 0;
  operand_2 = 0;
  result = 0;
 	calc_status.oper_1_minus_flag = 0;
	calc_status.oper_2_minus_flag = 0;
	calc_status.error_flag = 0;
	calc_status.operand_1_flag = 1;
	calc_status.operand_2_flag = 0;
	calc_status.first_char_flag = 1;
	calc_status.result_flag = 0;
	cur_oper = '\0';
	prev_oper = '\0';
	oper = '\0';
	input_num_chars = 0;
	memset(result_str, '\0', 7);
}