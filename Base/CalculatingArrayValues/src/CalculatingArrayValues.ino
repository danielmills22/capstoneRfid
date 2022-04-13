/*
 * Project CalculatingArrayValues
 * Description:
 * Author:
 * Date:
 */

#include <math.h>
#include <bits/stdc++.h>
using namespace std;

#include "stdio.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
/*
    Statistics - Demo for calculating Sensor Values
    1. find the mean (aka the average)
    2. find the max and min
    3. find the sum
    4. find the mode
    5. median (number that all other 
         numbers in the sequence fall to it's left and right)
       if the 
    6. range

*/

  float mathGrades[] = {75.0, 80.0, 85.0, 85.0, 86.0, 95.0, 97.0, 98.0};
  int length = sizeof(mathGrades) / sizeof(mathGrades[1]);

  void setup(){}

  void loop(){
    Serial.printf("Test");
    Serial.printf("\nSum: %0.1f", getSum(mathGrades, length));
    Serial.printf("\nMax: %0.1f", findMax(mathGrades, length));
  }


float getSum(float sequence[], int sequenceLength){
  int i = 0;
  float sum = 0.0;
  
  for(i=0;i<sequenceLength;i++){
      sum = sum + sequence[i];
  }
  return sum;
  //Serial.printf("Sum %.02f", sum);
}

float findMax(float sequence[], int sequenceLength){
  int i = 0;
  float max = 0.0;
  
  for(i=0;i<sequenceLength;i++){
    if(sequence[i] > max){
      max = sequence[i];
    }  
  }
  return max;
  //Serial.printf("Max %.02f", max);
}