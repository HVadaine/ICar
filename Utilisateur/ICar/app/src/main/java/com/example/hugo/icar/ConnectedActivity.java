package com.example.hugo.icar;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewDebug;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.SeekBar.OnSeekBarChangeListener;

import org.w3c.dom.Text;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;


/**
 * Created by hugo on 06/03/2016.
 */
public class ConnectedActivity extends Activity{


    private Socket socket;
    final int ID_FREIN=1,ID_VAR=2,ID_DIR=3;
    int SERVER_PORT = 6001;
    String SERVER_IP = "192.168.0.21";

    SeekBar seekFrein,seekDir,seekVar;
    TextView textFrein, textDir, textVar;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.connected_activity);
        Intent intent = getIntent();
        SERVER_PORT = intent.getIntExtra("Port", 6001);
        SERVER_IP = intent.getStringExtra("ip");
        new Thread(new ClientThread()).start();
        seekFrein = (SeekBar) findViewById(R.id.frein) ;
        seekDir = (SeekBar) findViewById(R.id.Dir) ;
        seekVar = (SeekBar) findViewById(R.id.Variateur) ;
        textFrein = (TextView) findViewById(R.id.TextFreinValue) ;
        textDir = (TextView) findViewById(R.id.TextDirValue) ;
        textVar = (TextView) findViewById(R.id.TextVarValue) ;

        setSeekBarListener(seekFrein,textFrein,ID_FREIN);
        setSeekBarListener(seekVar,textVar,ID_VAR);
        setSeekBarListener(seekDir,textDir,ID_DIR);
        /*Le bouton le plus important*/
        final Button STOP_BUTTON = (Button) findViewById(R.id.STOP);

        STOP_BUTTON.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                PrintWriter out = null;
                try {
                    out = new PrintWriter(new BufferedWriter(
                            new OutputStreamWriter(socket.getOutputStream())),
                            true);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                String msg = "S";//je peut envoyer n'importe quoi au final si le message n'est pas formaté correctement la voiture se met en arret d'urgence
                out.print(msg);
                out.flush();
                try {
                    socket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                STOP_BUTTON.setEnabled(false);
                finish();
                return false;
            }
        });
    }

    void setSeekBarListener(final SeekBar seekBar,final TextView text, final int id)
    {
        seekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
            char progressChanged = 0;

            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged = (char) progress;
//                text.setText(progress);
                send(id,progress);

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    void send(int header, int value)
    {
        PrintWriter out = null;
        try {
            out = new PrintWriter(new BufferedWriter(
                    new OutputStreamWriter(socket.getOutputStream())),
                    true);
        } catch (IOException e) {
            e.printStackTrace();
        }
        out.print((char) header);
        out.print((char) value);
        out.flush();
    }
    class ClientThread implements Runnable {

        @Override
        public void run() {

            try {
                InetAddress serverAddr = InetAddress.getByName(SERVER_IP);

                socket = new Socket(serverAddr, SERVER_PORT);


            } catch (UnknownHostException e1) {
                e1.printStackTrace();
                finish();
            } catch (IOException e1) {
                e1.printStackTrace();
                finish();
            }

        }

    }



}
