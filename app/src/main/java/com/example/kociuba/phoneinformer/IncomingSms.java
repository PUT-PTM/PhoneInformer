package com.example.kociuba.phoneinformer;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.telephony.SmsMessage;
import android.widget.Toast;

public class IncomingSms extends BroadcastReceiver {

    // Get the object of SmsManager
    final SmsManager sms = SmsManager.getDefault();

    public void onReceive(Context context, Intent intent) {

        // Retrieves a map of extended data from the intent.
        final Bundle bundle = intent.getExtras();


        if (bundle != null) {

            final Object[] pdusObj = (Object[]) bundle.get("pdus");

            for (int i = 0; i < pdusObj.length; i++) {

                SmsMessage currentMessage = SmsMessage.createFromPdu((byte[]) pdusObj[i]);
                String phoneNumber = currentMessage.getDisplayOriginatingAddress();

                String senderNum = phoneNumber;
                String message = currentMessage.getDisplayMessageBody();


                // Show Alert
                int duration = Toast.LENGTH_LONG;
                Toast toast = Toast.makeText(context,
                        "senderNum: " + senderNum + ", message: " + message, duration);
                toast.show();

                // getting the static instance of activity
                MainActivity activity = MainActivity.instance;


                if (activity != null) {
                    // we are calling here activity's method
                    activity.sms(senderNum);
                }

            } // end for loop
        } // bundle is null


    }
}