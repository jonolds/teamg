package terasic.spider;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class startdisplay extends Activity {
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.startdisplay);
		new Thread(new Runnable() {

			@Override
			public void run() {
				startActivity(new Intent().setClass(startdisplay.this, BluetoothChat.class));
				startdisplay.this.finish();
			}
		}).start();
		/*
		Button button = (Button)findViewById(R.id.button01); 
		button.setOnClickListener(new Button.OnClickListener(){
		@Override
		public void onClick(View v) {
		// TODO Auto-generated method stub 
		Intent intent = new Intent();
		intent.setClass(startdisplay.this, BluetoothChat.class);
		startActivity(intent); 
		startdisplay.this.finish(); 
		}
		});
		}
		*/
	}
}
