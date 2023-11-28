package com.lucasta.pickle;

public class NativeLoader extends android.app.NativeActivity {
  static { System.loadLibrary("main"); }
}
