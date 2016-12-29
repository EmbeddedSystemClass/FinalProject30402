import sys
from Adafruit_IO import Client

# Usage:
#  python send-aio.py feedname value
#
# SECURE MODIFICATION PROCEDURE
# 1. Check out the latest version of this file from the Github project
# 2. Edit this file to add the AIO key on the line aiokey = ...
# 3. Save this version to disk
# 4. Send to the Yun via scp in Git Bash as follows:
#     scp send-aio.py username@arduinoname.local:/arduino
# 5. Remove this version of the file from Git by checking out the original version again.
#    WARNING: DO NOT SAVE PROPRIETARY INFO IN THE PUBLIC ACCESSIBLE GITHUB PROJECT!

aiokey = ''  #set this to your adafruit IO AIO key
io = Client(aiokey)

io.send(sys.argv[1], sys.argv[2])
