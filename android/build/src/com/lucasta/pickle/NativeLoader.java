package com.lucasta.pickle;

import android.content.Context;
import android.os.Bundle;
import android.text.InputType;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;

public class NativeLoader extends android.app.NativeActivity
{
	static
	{
		System.loadLibrary("main");
	}

	private NativeInputView inputView;
	private native void nativeOnTextInput(String text);
	private native void nativeOnKeyDelete();

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		// Create a generic view to handle text input
		inputView = new NativeInputView(this);
		inputView.setFocusable(true);
		inputView.setFocusableInTouchMode(true);

		// Add it to the window (1x1 pixel, effectively invisible)
		ViewGroup.LayoutParams params = new ViewGroup.LayoutParams(1, 1);
		this.addContentView(inputView, params);
	}

	private class NativeInputView extends View
	{
		public NativeInputView(Context context)
		{
			super(context);
		}

		@Override
		public boolean onCheckIsTextEditor()
		{
			return true;
		}

		@Override
		public InputConnection onCreateInputConnection(EditorInfo outAttrs)
		{
			outAttrs.inputType = InputType.TYPE_CLASS_TEXT;
			outAttrs.imeOptions =
							EditorInfo.IME_FLAG_NO_FULLSCREEN | EditorInfo.IME_ACTION_DONE;

			return new BaseInputConnection(this, true) {
				@Override
				public boolean commitText(CharSequence text, int newCursorPosition)
				{
					for (int i = 0; i < text.length(); i++)
					{
						char c = text.charAt(i);
						if (c == '\n')
						{
							hideSoftKeyboard();
						}
						else
						{
							nativeOnTextInput(Character.toString(c));
						}
					}
					return super.commitText(text, newCursorPosition);
				}

				@Override
				public boolean deleteSurroundingText(int beforeLength, int afterLength)
				{
					if (beforeLength > 0)
					{
						nativeOnKeyDelete();
					}
					return super.deleteSurroundingText(beforeLength, afterLength);
				}

				@Override
				public boolean sendKeyEvent(KeyEvent event)
				{
					if (event.getAction() == KeyEvent.ACTION_DOWN
									&& event.getKeyCode() == KeyEvent.KEYCODE_DEL)
					{
						nativeOnKeyDelete();
						return true;
					}
					if (event.getAction() == KeyEvent.ACTION_DOWN
									&& event.getKeyCode() == KeyEvent.KEYCODE_ENTER)
					{
						hideSoftKeyboard();
						return true;
					}
					return super.sendKeyEvent(event);
				}
			};
		}
	}

	public void showSoftKeyboard()
	{
		final NativeLoader activity = this;
		this.runOnUiThread(new Runnable() {
			@Override
			public void run()
			{
				if (activity.inputView != null)
				{
					activity.inputView.requestFocus();
					InputMethodManager imm =
									(InputMethodManager) activity.getSystemService(
													Context.INPUT_METHOD_SERVICE);
					if (imm != null)
					{
						imm.showSoftInput(
										activity.inputView, InputMethodManager.SHOW_FORCED);
					}
				}
			}
		});
	}

	public void hideSoftKeyboard()
	{
		final NativeLoader activity = this;
		this.runOnUiThread(new Runnable() {
			@Override
			public void run()
			{
				if (activity.inputView != null)
				{
					InputMethodManager imm =
									(InputMethodManager) activity.getSystemService(
													Context.INPUT_METHOD_SERVICE);
					if (imm != null)
					{
						imm.hideSoftInputFromWindow(activity.inputView.getWindowToken(), 0);
					}
				}
			}
		});
	}
}
