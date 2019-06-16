一、背景

众所周知，Android JNI层的Crash问题是个比较头疼的问题。相对Java层来说，由于c/c++造成的crash没有输出如同

Java的Exception Strace，所以crash的定位问题是件比较艰难的事情。Google Breakpad是一套完整的工具集，从crash的

捕获到crash的dump，都提供了相对应的工具。

二、目的:当程序crash了之后，收集程序崩溃信息，定位到崩溃的地方

三、方案

Google breakpad是一个跨平台的崩溃转储和分析框架和工具集合。

Breakpad由三个主要组件：

    client，以library的形式内置在你的应用中，当崩溃发生时写 minidump文件
    
    symbol dumper, 读取由编译器生成的调试信息（debugging information），并生成 symbol file
    
    processor， 读取 minidump文件 和 symbol file ，生成可读的c/c++ Stack trace.

简单来说就是一个生成 minidump，一个生成symbol file，然后将其合并处理成可读的Stack trace。

四、集成

1.克隆或者下载breakpad开源库：https://github.com/google/breakpad.git

2.进入根目录执行：

`./configure && make`

 src/client/linux目录下生成libbreakpad_client.a 静态库文件[libbreakpad_client.a](/uploads/33f43e7fad5ec0fa5358c5fc9373e1db/libbreakpad_client.a)，src/processor 目录下生成

minidump_stackwalk 工具[minidump_stackwalk](/uploads/8b56c60770516b83344378e22ab64874/minidump_stackwalk)，用于导出crash log，

src/tools/linux/dump_syms 目录下生成 dump_syms工具[dump_syms](/uploads/05ca888d36dd5dfe4b0927e881ae775c/dump_syms)，用于导出符号文件。

3.集成的方式有两种, 参考breakpad目录下README.ANDROID。

注：这里直接使用sample_app,如需在app中使用,可以直接include sample_app中jin的android.mk文件

(1) 编译成成.a库文件, 直接用加入.a的方式来集成

(2) 使用import-module的方式, 让ndk来编译

这里采用第(2)种方式，详见sample_app目录下的jin中的android.mk文件[Android.mk](/uploads/bf07e5cc4ce46a87045203239a5eade4/Android.mk)

(1) 直接使用/android/sample_app的实例

(2) 在jni/Android.mk中加入代码

```
LOCAL_CPPFLAGS  := -pie -fPIE -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS

LOCAL_LDFLAGS   := -pie -fPIE
```

注:详见sample-app文件夹下的jin文件夹下的android.mk

(3) 在main.cpp 中加入代码[test_breakpad.cpp](/uploads/6becceec0fad8db20eda02f623ef9866/test_breakpad.cpp)

#include <stdio.h>

#include "client/linux/handler/exception_handler.h"

#include "client/linux/handler/minidump_descriptor.h"

namespace {

bool DumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,void* context,

  bool succeeded) {printf("Dump path: %s\n", descriptor.path());

  return succeeded;

}

void Crash() {

  volatile int* a = reinterpret_cast<volatile int*>(NULL);

  *a = 1;

}

}  // namespace

int main(int argc, char* argv[]) {

  google_breakpad::MinidumpDescriptor descriptor("/sdcard");

  google_breakpad::ExceptionHandler eh(descriptor, NULL, DumpCallback, NULL, true, -1);

  Crash();

  return 0;

}
此时, 就可以编译android cocos2d-x工程了, 生成的crash文件放在 /sdcard下, 文件后缀为dmp


五、测试

Build instructions:

   ```
cd android/sample_app

   $ndk-build
   ```

  Where $NDK points to a valid Android NDK installation.After executing "ndk-build"

,there will be many binary files named test_google_breakpad

Usage instructions:

   After buildind the test program, send it to a device, then run it as

   the shell UID:

     adb push libs/armeabi/test_google_breakpad /data/local/tmp
    
     adb shell /data/local/tmp/test_google_breakpad

ps：However, you don"t have do these two steps, in that our shell of 'run_test' already has included them.

   This will simply crash after dumping the name of the generated minidump

   file.[a1ee9a8a-21bc-4e2b-85046fab-20468514.dmp](/uploads/52f61416273700a479a323df4d382412/a1ee9a8a-21bc-4e2b-85046fab-20468514.dmp)

   use the command of adb to pull the file of .dmp 

   See jni/test_breakpad.cpp for details.

六、结果分析

Note: all of the things you need do is that to use the command of adb to pull the file of .dmp and to analysis it.

在sample_app目录下新建Dump文件，拷贝命令dump_syms和minidump_stackwalk到Dump目录下。

```
cp breakpad-master/src/tools/linux/dump_syms/dump_syms ./Dump

cp breakpad-master/src/processor/minidump_stackwalk ./Dump
```

生成sym文件[test_google_breakpad.sym](/uploads/54fa8d709f89fa17e5ba027ce44dc7c1/test_google_breakpad.sym)

```
cd Dump

./dump_syms test_google_breakpad > test_google_breakpad.sym
```

注：在我们的APP中，test_google_breakpad在obj/local/armeabi-v7a目录下，需要将其copy到Dump目录下

在Dump中新建symbols文件夹，在symbols文件夹下新建test_google_breakpad文件夹，然后在

test_google_breakpad文件夹下新建以test_google_breakpad.sym文件中的第一行的十六进制字符串命名的文件

夹，将test_google_breakpad.sym移动到这个十六进制字符串的文件夹下。

生成crashed.log文件[crashed.log](/uploads/79a0fdf3aebf28307c9caf98dba1d559/crashed.log)


```
cd Dump

 ./minidump_stackwalk xxxx-xxxx-xxxxx-xxxx.dmp symbols > crashed.log
```

注:确保dmp文件和symbols目录是同级的

打开crashed.log文件，查看程序崩溃的地方
