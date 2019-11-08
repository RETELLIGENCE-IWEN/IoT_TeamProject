from __future__ import print_function
  
import json
import boto3
  
print('Loading function')
  
def lambda_handler(event, context):
  sensorValue=event['sensorValue']
  if sensorValue=='unstable':
    message='ALERT! Someone is in your house!'
    
  # Parse the JSON message 
  eventText = json.dumps(event)
  
  print('Received event: ', eventText)
  
  # Create an SNS client
  sns = boto3.client('sns')
  
  # Publish a message to the specified topic
  response = sns.publish (
    TopicArn = 'arn:aws:sns:ap-northeast-2:677003799806:homeblackbox',
    Message = message
  )
  
  print(response)
