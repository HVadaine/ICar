package com.example.hugo.icar;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.io.IOException;
import java.net.UnknownHostException;
import java.util.List;

public class MainActivity extends Activity{
    EditText t,p;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // On récupère le bouton par son identBifiant
        Button b = (Button) findViewById(R.id.buttonC);
        t=(EditText) findViewById(R.id.ip);
        p=(EditText) findViewById(R.id.port);
        b.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent secondeActivite = new Intent(MainActivity.this, ConnectedActivity.class);
                // On rajoute un extra ou pas
                secondeActivite.putExtra("ip",t.getText().toString());
                secondeActivite.putExtra("Port", Integer.parseInt(p.getText().toString()));
                // Puis on lance l'intent !
                startActivity(secondeActivite);
            }
        });
    }


}

