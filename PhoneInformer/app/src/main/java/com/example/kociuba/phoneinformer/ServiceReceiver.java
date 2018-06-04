package com.example.kociuba.phoneinformer;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.widget.Toast;

public class ServiceReceiver extends BroadcastReceiver {


    boolean flag = true;
    boolean flag2 = true;
    int a =0;

    String TAG = "ServiceReciver";
    @Override
    public void onReceive(final Context context,final Intent intent) {
        TelephonyManager telephony = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
        telephony.listen(new PhoneStateListener(){
            @Override
            public void onCallStateChanged(int state, String incomingNumber) {
                super.onCallStateChanged(state, incomingNumber);
                System.out.println("incomingNumber : "+incomingNumber);

                String stateStr = intent.getExtras().getString(TelephonyManager.EXTRA_STATE); // stan dla konczenia rozmow

                int duration = Toast.LENGTH_LONG;
               /* Toast toast = Toast.makeText(context,
                        "incomingNumber: "+ incomingNumber , duration);
                toast.show();*/

                // getting the static instance of activity
                MainActivity activity = MainActivity.instance;


                if (activity != null && flag == true) {
                    // we are calling here activity's method
                    if (stateStr.equals(TelephonyManager.EXTRA_STATE_RINGING)) {
                        Log.d(TAG, "Call function");
                        activity.call(incomingNumber);
                        flag = false;
                    }
                }

                if (flag2 == true && stateStr.equals(TelephonyManager.EXTRA_STATE_IDLE)){ // warunek konczenia
                    activity.decall();
                    Log.d(TAG, "DECall function");
                    flag2 =false;
                }

            }
        },PhoneStateListener.LISTEN_CALL_STATE);
    }
}