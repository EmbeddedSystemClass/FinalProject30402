import sys
from twilio.rest import TwilioRestClient

# Usage:
#  python send-sms.py StringToSendAsSMSText
#
# SECURE MODIFICATION PROCEDURE
# 1. Check out the latest version of this file from the Github project
# 2. Edit this file to add the appropriate SID and auth token, as well as phone numbers, below
# 3. Save this version to disk
# 4. Send to the Yun via scp in Git Bash as follows:
#     scp send-sms.py username@arduinoname.local:/arduino
# 5. Remove this version of the file from Git by checking out the original version again.
#    WARNING: DO NOT SAVE PROPRIETARY INFO IN THE PUBLIC ACCESSIBLE GITHUB PROJECT!

# Insert appropriate Twilio account SID and auth token HERE
account_sid=""
auth_token=""
test_sid=""
test_token=""
# Insert the Twilio phone number and number to call here
# Use the format +1 (or other country code) and 10 digits as needed w/o other punctuation
twilio_phone_number="+16695555555"
cellphone="+16505555555"

client = TwilioRestClient(account_sid, auth_token)
#client = TwilioRestClient(test_sid, test_token)

client.messages.create(to=cellphone, from_=twilio_phone_number, body=sys.argv[1])
