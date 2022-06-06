import vonage
import random
import time
import serial

otp_sent = False

def create_otp():
    listt = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'A', 'B', 'C', 'D', '*', '#']
    keyword = [random.choice(listt) for i in range(5)]
    return ''.join(keyword)



def send_sms_rmn(msg = None):
    global otp_sent
    OTP = create_otp()
    otp = OTP
    client = vonage.Client(key="********", secret="****************")
    sms = vonage.Sms(client)

    responseData = sms.send_message(
        {
            "from": "Vonage APIs",
            "to": "910000000000",
            "text": f"Your OTP is: {otp}",
        }
    )

    if responseData["messages"][0]["status"] == "0":
        print("Message sent successfully.")
    else:
        print(f"Message failed with error: {responseData['messages'][0]['error-text']}")
    otp_sent = True

    return OTP


try:
    arduino = serial.Serial("COM14", 9600, timeout=1)
    print("connected")
except:
    print("Please check port")


connection = "."
done = 'D'
authorised = 'A'
locked = 'L'
incorrect_otp = 'I'

while True:
    arduino.write(connection.encode())
    msg = arduino.read().decode('ascii')
    print(msg)
    #print(len(msg))

        
    if (msg == '1' and otp_sent == False):
        OTP = send_sms_rmn()
        print(OTP)
        break

re_otp = 0
while True:
    arduino.write(done.encode())
    msg = arduino.read().decode('ascii')
    print(msg)

    if msg == 'K':
        key_arr = [arduino.read().decode('ascii') for i in range(5)]
        print(''.join(key_arr))

        if (''.join(key_arr) == OTP):
            print('authorized')
            arduino.write(authorised.encode())
            
        elif (re_otp <= 2):
            print('Incorrect OTP, Sending new one')
            time.sleep(3.0)
            arduino.write(incorrect_otp.encode())
            OTP = send_sms_rmn()
            print(OTP)
            re_otp += 1
            continue
        
        elif (re_otp > 2):
            print('Bag locked')
            arduino.write(locked.encode())
            send_sms_rmn(msg = 'Bag Locked.')


arduino.close()
