import dropbox
import datetime

# Usage:
#  python send-sms.py StringToSendAsSMSText
#
# SECURE MODIFICATION PROCEDURE
# 1. Check out the latest version of this file from the Github project
# 2. Edit this file to add the appropriate generated access token below
# 3. Save this version to disk
# 4. Send to the Yun via scp in Git Bash as follows:
#     scp upload-to-dropbox.py username@arduinoname.local:/arduino
# 5. Remove this version of the file from Git by checking out the original version again.
#    WARNING: DO NOT SAVE PROPRIETARY INFO IN THE PUBLIC ACCESSIBLE GITHUB PROJECT!

suffix = datetime.datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
filename = "yunpic-" + suffix + ".jpg"

client = dropbox.client.DropboxClient('YOURACCESSTOKEN')
f = open('/arduino/pic.jpg', 'rb')
response = client.put_file(filename, f)
print "uploaded:", response
