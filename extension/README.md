<div align="center" ><font size="70">Eigen优化入门</font></div>

<details>
<summary><font size=5>VS Code开发环境配置</font></summary>

## 软件及其插件安装(windows)
1.安装 [Visual Studio Code](https://code.visualstudio.com/download)  
2.安装 VS Code中 C++ 插件，通过<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>X</kbd>打开插件窗口，搜索 C++ 关键词  
<img src="extension/img/cpp_extension.png" alt="图片无法加载" width="900" align=center />  

**NOTE:如有需要，可安装中文语言包和 文件传输插件sftp(用于在服务器和本地之间传输文件)，如下：**  
<img src="extension/img/Chinese.PNG" alt="图片无法加载" width="900" align=center />  
<img src="extension/img/sftp.PNG" alt="图片无法加载" width="900" align=center />  

3.安装 c++ 编译器(MinGW-w64)  
1.安装包下载  
方法一：下载[Installer](http://www.mingw-w64.org/doku.php)  在线安装(费时)  

方法二：下载[离线包](https://sourceforge.net/projects/mingw-w64/files/)(较快)
点击[离线包下载](https://sourceforge.net/projects/mingw-w64/files/)进入 进入下图1后将页面往下滑到图2区域，点击所需离线包名称(参照方法一第三步标注)，完成下载操作。
<img src="extension/img/MinGW_install1.jpeg" width="900" alt="图片无法加载">

<img src="extension/img/MinGW_install2.jpeg" width="900" alt="图片无法加载">

解压得到二进制文件，文件结构如下：

<img src='extension/img/MinGW_install_decompress.png' alt="图片无法加载">

2.环境变量配置  
<u>右键计算机→属性→高级系统设置→高级→环境变量</u>，然后鼠标双击系统变量中Path，或者选中后点击编辑，在变量值输入框的末尾输入英文分号后将MinGW-w64包目录下bin文件夹的全路径粘贴到后面，bin的后面有无斜杠均可；或者选择Path后新建，复制bin文件夹路径，如下图：

<img src="extension/img/MinGW_path.PNG" width="900" alt="图片无法下载">

在window cmd中键入 gcc/g++ -v 查看编译器版本

<img src="extension/img/gcc_version.png" width="900" alt="图片无法下载">

**拓展阅读**：  
MinGW 的全称是：Minimalist GNU on Windows 。它实际上是将经典的开源 C语言 编译器 GCC 移植到了 Windows 平台下，并且包含了 Win32API ，因此可以将源代码编译为可在 Windows 中运行的可执行程序。而且还可以使用一些 Windows 不具备的，Linux平台下的开发工具。一句话来概括：MinGW 就是 GCC 的 Windows 版本 。
MinGW-w64 与 MinGW 的区别在于 MinGW 只能编译生成32位可执行程序，而 MinGW-w64 则可以编译生成 64位 或 32位 可执行程序。
正因为如此，MinGW 现已被 MinGW-w64 所取代，且 MinGW 也早已停止了更新，内置的 GCC 停滞在了 4.8.1 版本。  

4.CMake 和Make 安装(可选)    
    TODO

## VS Code调试环境配置
在VS Code的工程中(通常指一个文件夹或者workspace),通常需要包含c_cpp_properties.json、task.json、和launch.json 三个文件，如果已安装sftp插件，还会有sftp.json文件，如下图所示。在VS Code中，所有的设置都可以通过JSON文件来完成，下面我们将对这些文件进行配置，从而实现代码调试功能。

<center>
<img src="extension/img/vs_code_dir.PNG" alt="图片无法加载" >
</center>

1.. c_cpp_properties.json( compiler path and IntelliSense settings )配置  
如果VS Code打开的文件夹中没有c_cpp_properties.json文件，可通过<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>P</kbd>调出code的指令窗口，键入`Edit Configurations`并选择对应编辑设置，本次我们使用一个新建的文件夹进行测试，发现code设置后该目录下生成了.vscode文件夹和c_cpp_properties.json文件，如下：  
<img src="extension/img/edit_configurations.PNG"  alt="图片无法加载">
<img src="extension/img/edit_configurations_done.PNG" alt="图片无法加载">

c_cpp_properties.json文件设置内容如下，其中***includePath***为IntelliSense在搜索所包含的 头文件 时要使用的路径列表，此处可理解为 告诉 编辑器(code)去哪里找这些.h文件，从而完成代码自动补全和检查功能。***compilerPath***需要指定到前文安装的编译器(gcc/g++ etc).  
 
```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "compilerPath": "D:\\mingw64\\bin\\gcc.exe",
            "cStandard": "c11",
            "cppStandard": "gnu++14",
            "intelliSenseMode": "clang-x64"
        }
    ],
    "version": 4
}
```  

2.tasks.json( build instructions )配置  
通过<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>P</kbd>调出code指令窗口，键入`configure tasks`并选择对应编辑设置。  

<img src="extension/img/configure_tasks.PNG" alt="图片无法加载">
<img src="extension/img/configure_tasks_done.PNG" width="900" alt="图片无法加载" >

tasks.json 内容设置如下，其中***command***为编译所需要的编译器(gcc/g++ etc),***args***为编译器编译时参数，在需要使用一些第三方头文件时，需要在此处通过`-I /path/include`进行指定，从而告诉编译器去哪里寻找对于头文件：    
```json
{
    "tasks": [
        {
            "type": "shell",
            "label": "C/C++: g++.exe build active file",
            "command": "D:\\mingw64\\bin\\g++.exe",
            "args": [
                "-g",
                "${file}",
                "-o",
                "${fileDirname}\\${fileBasenameNoExtension}.exe"
            ],
            "options": {
                "cwd": "${workspaceFolder}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ],
    "version": "2.0.0"
}
```  

3.launch.json( debugger setting )配置  
通过<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>P</kbd>调出code指令窗口，键入`open launch.json`并选择对应编辑设置。  

<img src='extension/img/open_launch.PNG' alt="图片无法加载">
<img src="extension/img/open_launch_done.PNG" width="900" alt="图片无法加载">

launch.json文件设置如下，其中`program`为生成的目标文件，`miDebuggerPath`指定系统中debugger路径：  

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "g++.exe - 生成和调试活动文件",
            "type": "cppdbg",
            "request": "launch",
            "program": "${fileDirname}\\${fileBasenameNoExtension}.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "D:\\mingw64\\bin\\gdb.exe",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: g++.exe build active file"
        }
    ]
}
```  

4.sftp.json配置(仅安装sftp插件的需要进行配置)  
通过<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>P</kbd>调出code指令窗口，键入`sftp :config` 并选择对应编辑设置。stfp.json设置如下：  
```json
{
    "name": "name tag",
    "host": "your server ip",
    "protocol": "sftp",
    "port": 22,
    "username": "your username",
    "password":"your password",
    "remotePath": "/home/yourPath/",
    "uploadOnSave": true,
    "ignore":[
        ".vscode",
        ".git",
        ".DS_Store",
        "**/build**"
    ]
}
```
设置与不同服务器进行同步（通过profiles实现）：
```json
{
  "name":"eigen",
  "context": "D:\\Program\\huawei\\eigen",
  "uploadOnSave": true,
  "ignore":[
      ".vscode",
      ".git",
      ".DS_Store",
      "**/build/**",
      "**/build_vs/**",
      "*.exe"
  ],
  "remotePath": "/home/qiguoqiang/huawei/eigen/",
  "profiles": {
      "openEuler":{
          "host": "119.3.188.37",
          "username": "qiguoqiang",
          "password": "qiang_123"
      },
      "x86":{
          "host": "121.37.13.254",
          "username": "qiguoqiang",
          "password": "qiguoqiang123"
      }

  }
}
```
  

**拓展阅读**  
预定义变量  
`${workspaceRoot}` -VSCode中打开文件夹的路径 the path of the folder opened in VS Code  
`${workspaceFolder}` -在VS Code中打开的文件夹的路径  
`${workspaceFolderBasename}`-在VS Code中打开的文件夹名称，不带任何斜杠（/）  
`${file}` -当前打开的文件  
`${relativeFile}` -相对于当前打开的文件workspaceFolder  
`${relativeFileDirname}` -相对于当前打开的文件的目录名workspaceFolder  
`${fileBasename}` -当前打开的文件的基本名称  
`${fileBasenameNoExtension}`-当前打开的文件的基本名称，没有文件扩展名  
`${fileDirname}`-当前打开的文件的目录名  
`${fileExtname}` -当前打开的文件的扩展名  
`${cwd}` -启动时任务运行器的当前工作目录  
`${lineNumber}` -活动文件中当前选择的行号  
`${selectedText}` -活动文件中的当前选定文本  
`${execPath}`-正在运行的VS Code可执行文件的路径  
`${defaultBuildTask}` -默认构建任务的名称  

</details>

<details>
<summary><font size=5>编译原理</font></summary>
 
### 编译原理   
### GCC编译器  
### CMake 与 Make  
</details>

<details>
<summary><font size=5> SIMD优化与NEON指令集</font></summary>

// this part was reference to ARM official document [introducing-neon-for-armv8-a](https://developer.arm.com/architectures/instruction-sets/simd-isas/neon/neon-programmers-guide-for-armv8-a/introducing-neon-for-armv8-a)

### SIMD技术和NEON指令集结构
所谓的SIMD指令，指的是single instruction multiple data，即单指令多数据运算，其目的就在于帮助CPU实现数据并行，提高运算效率。  
<img src="extension/img/SIMD.png" width=300 alt="图片无法加载">  
而NEON是SIMD技术在ARM结构系列芯片上的实现，其提供了原有ARM 指令集结构之外的拓展指令集及结构（额外的NEON寄存器）。NEON指令集结构将对多媒体领域的音视频编解码、用户接口、2D/3D图形或游戏进行加速，提示用户体验。同时，Neon还可以加速信号处理算法和功能，以加快诸如音频和视频处理，语音和面部识别，计算机视觉和深度学习之类的应用程序。  
从软件开发者的角度来看，主要有四种方法将NEON技术应用到我们的项目中：  
1.使用ARM的开源计算库，如 Arm Compute Library .   
2.使用编译器的自动向量化选项，支持的编译器:Arm Compiler 6、Arm C/C++ Compiler、LLVM-clang 、GCC  
3. NEON intrinisic  
4. hand-coded NEON assembler

### SISD VS SIMD
SISD:大多数Arm指令都是单指令单数据（SISD）。每条指令在单个数据源上执行其指定的操作。因此，处理多个数据项需要多个指令。例如，要执行四个加法运算，需要四个指令来对四对寄存器的值进行相加： 
```
ADD w0, w0, w5
ADD w1, w1, w6
ADD w2, w2, w7
ADD w3, w3, w8
```
此方法相对较慢,为了提高性能和效率，通常使用专用处理器（如GPU）来处理多媒体数据任务，这些处理器可以用一条指令处理多个数据值。如果您要处理的值小于最大位大小，则SISD指令会浪费该额外的潜在带宽。例如，将8位值相加时，需要将每个8位值加载到单独的64位寄存器中。由于处理器，寄存器和数据路径都是为64位计算而设计的，因此在小数据量上执行大量的单独操作无法有效利用机器资源。   
SIMD:单指令多数据（SIMD）指令对多个数据项同时执行相同的操作。这些数据项在较大的寄存器中打包为单独的通道。例如，以下指令将四对单精度（32位）值相加。但是，在这种情况下，将值打包到两对128位寄存器中的单独通道中。然后，将第一个源寄存器中的每个通道添加到第二个源寄存器中的相应通道，然后再存储在目标寄存器中的同一通道中：
```
ADD V10.4S, V8.4S, V9.4S
// This operation adds two 128-bit (quadword) registers, V8 and V9,
// and stores the result in V10.
// Each of the four 32-bit lanes in each register is added separately.
// There are no carries between the lanes.
```
<img src="extension/img/Neon-Simd-Add-Example.png" width=500 alt="图片无法加载">  

该图显示了128位寄存器，每个寄存器包含四个32位值，但是Neon寄存器也可以使用其他组合：

* Sixteen 8-bit elements (operand suffix .16B, where B indicates byte)
* Eight 16-bit elements (operand suffix .8H, where H indicates halfword)
* Four 32-bit elements (operand suffix .4S, where S indicates word)
* Two 64-bit elements (operand suffix .2D, where D indicates doubleword)
 
也可以只使用Neon寄存器的低64位，在这种情况下，未使用Neon寄存器的高64位：
* Eight 8-bit elements (operand suffix .8B, where B indicates byte)
* Four 16-bit elements (operand suffix .4H, where H indicates halfword)
* Two 32-bit elements (operand suffix .2S, where S indicates word)  

<img src="extension/img/Neon-reg.png" width=600 alt="图片无法加载">

注意，图中所示的加法运算对于每个通道来说都是真正独立的。通道0的任何溢出或进位都不会影响通道1，这是完全独立的计算。

### NEON intrinsic
Neon intrinsic是NEON的C/C++接口，这些函数在arm_neon.h中定义，通过使用intrinsic函数开发者可以通过c/c++编程的方式而无需直接编写汇编代码来使用neon技术（register allocation and pipeline optimization are handled by the compiler）。此外，intrinsic函数在编译时会被内联到调用代码中，减少了函数调用的额外开支，可以达到近乎hand-coded assembler的优化效果。使用Neon内部函数有很多好处：     
* 强大：内在的特性使程序员可以直接访问Neon指令集，而无需手写的汇编代码。
* 便携式：可能需要针对不同的目标处理器重新编写NEON assembler指令。Neon intrinsic的C/C++代码可以为新目标或新的执行状态（例如，从AArch32迁移到AArch64）进行编译，而只需很少的代码更改或没有代码更改。
* 灵活：程序员可以在需要时使用Neon，在不需要时使用C/C++，同时避免了许多底层工程问题。

但是，内在函数可能并非在所有情况下都是最好的选择：
* 与导入库或依赖编译器相比，NEON intrinsic学习成本更高。
* 手工优化的汇编代码可能会提供最大的性能改进范围，即使其更难编写。

使用NEON intrinsic 重写上面的加法运算为：
```
// type of a and b both float32x4_t 
float32x4_t result ;
result = vaddq_f32 (a, b);
```
### Macros
NEON intrinsic 需要硬件和编译器的支持，在不同情况下编译时使用的不同选项也影响着intrinsic函数是否可用，因此我们通常根据宏定义来判断当前环境下是否支持neon intrinsic：
* __ARM_NEON || \_\_ARM_NEON\_\_
    * Advanced SIMD is supported by the compiler
    * Always 1 for AArch64
* __ARM_NEON_FP
    * Neon floating-point operations are supported
    * Always 1 for AArch64
* __ARM_FEATURE_CRYPTO
    * Crypto instructions are available.
    * Cryptographic Neon intrinsics are therefore available.
* __ARM_FEATURE_FMA
    * The fused multiply-accumulate instructions are available.
    * Neon intrinsics which use these are therefore available.

### 矩阵相乘实例

<img src="extension/img/matrix_multiplication.png" width=400 alt="图片无法加载">

对于单精度浮点数（float32）类型的矩阵相乘c/c++ 实现如下：
```c++
// assumed a column-major layout of the matrices in memory.
// That is, an n x m matrix M is represented as an array M_array where Mij = M_array[n*j + i].
void matrix_multiply_c(float32_t *A, float32_t *B, float32_t *C, uint32_t n, uint32_t m, uint32_t k) {
    for (int i_idx=0; i_idx < n; i_idx++) {
        for (int j_idx=0; j_idx < m; j_idx++) {
            C[n*j_idx + i_idx] = 0;
            for (int k_idx=0; k_idx < k; k_idx++) {
                C[n*j_idx + i_idx] += A[n*k_idx + i_idx]*B[k*j_idx + k_idx];
            }
        }
    }
}
```
由于NEON 寄存器为128bit大小，可同时对四个float32数据进行操作，因此可将矩阵乘法分解为多个4x4大小矩阵进行计算，NEON intrinsic实现如下：
```c++
void matrix_multiply_4x4_neon(float32_t *A, float32_t *B, float32_t *C) {
	// these are the columns A
	float32x4_t A0;
	float32x4_t A1;
	float32x4_t A2;
	float32x4_t A3;
	
	// these are the columns B
	float32x4_t B0;
	float32x4_t B1;
	float32x4_t B2;
	float32x4_t B3;
	
	// these are the columns C
	float32x4_t C0;
	float32x4_t C1;
	float32x4_t C2;
	float32x4_t C3;
	
	A0 = vld1q_f32(A);
	A1 = vld1q_f32(A+4);
	A2 = vld1q_f32(A+8);
	A3 = vld1q_f32(A+12);
	
	// Zero accumulators for C values
	C0 = vmovq_n_f32(0);
	C1 = vmovq_n_f32(0);
	C2 = vmovq_n_f32(0);
	C3 = vmovq_n_f32(0);
	
	// Multiply accumulate in 4x1 blocks, i.e. each column in C
	B0 = vld1q_f32(B);
	C0 = vfmaq_laneq_f32(C0, A0, B0, 0);
	C0 = vfmaq_laneq_f32(C0, A1, B0, 1);
	C0 = vfmaq_laneq_f32(C0, A2, B0, 2);
	C0 = vfmaq_laneq_f32(C0, A3, B0, 3);
	vst1q_f32(C, C0);
	
	B1 = vld1q_f32(B+4);
	C1 = vfmaq_laneq_f32(C1, A0, B1, 0);
	C1 = vfmaq_laneq_f32(C1, A1, B1, 1);
	C1 = vfmaq_laneq_f32(C1, A2, B1, 2);
	C1 = vfmaq_laneq_f32(C1, A3, B1, 3);
	vst1q_f32(C+4, C1);
	
	B2 = vld1q_f32(B+8);
	C2 = vfmaq_laneq_f32(C2, A0, B2, 0);
	C2 = vfmaq_laneq_f32(C2, A1, B2, 1);
	C2 = vfmaq_laneq_f32(C2, A2, B2, 2);
	C2 = vfmaq_laneq_f32(C2, A3, B2, 3);
	vst1q_f32(C+8, C2);
	
	B3 = vld1q_f32(B+12);
	C3 = vfmaq_laneq_f32(C3, A0, B3, 0);
	C3 = vfmaq_laneq_f32(C3, A1, B3, 1);
	C3 = vfmaq_laneq_f32(C3, A2, B3, 2);
	C3 = vfmaq_laneq_f32(C3, A3, B3, 3);
	vst1q_f32(C+12, C3);
}
```
最终的矩阵乘法NEON intrinsic实现如下（矩阵大小需为4的整数倍，如不满足，可通过扩充0达到要求）：
```cpp
void matrix_multiply_neon(float32_t  *A, float32_t  *B, float32_t *C, uint32_t n, uint32_t m, uint32_t k) {
	/* 
	 * Multiply matrices A and B, store the result in C. 
	 * It is the user's responsibility to make sure the matrices are compatible.
	 */	

	int A_idx;
	int B_idx;
	int C_idx;
	
	// these are the columns of a 4x4 sub matrix of A
	float32x4_t A0;
	float32x4_t A1;
	float32x4_t A2;
	float32x4_t A3;
	
	// these are the columns of a 4x4 sub matrix of B
	float32x4_t B0;
	float32x4_t B1;
	float32x4_t B2;
	float32x4_t B3;
	
	// these are the columns of a 4x4 sub matrix of C
	float32x4_t C0;
	float32x4_t C1;
	float32x4_t C2;
	float32x4_t C3;
	
	for (int i_idx=0; i_idx<n; i_idx+=4) {
            for (int j_idx=0; j_idx<m; j_idx+=4){
                 // zero accumulators before matrix op
                 C0=vmovq_n_f32(0);
                 C1=vmovq_n_f32(0);
                 C2=vmovq_n_f32(0); 
                 C3=vmovq_n_f32(0);
                 for (int k_idx=0; k_idx<k; k_idx+=4){
                    // compute base index to 4x4 block
                    A_idx = i_idx + n*k_idx;
                    B_idx = k*j_idx + k_idx;

                    // load most current a values in row
                    A0=vld1q_f32(A+A_idx);
                    A1=vld1q_f32(A+A_idx+n);
                    A2=vld1q_f32(A+A_idx+2*n);
                    A3=vld1q_f32(A+A_idx+3*n);

                    // multiply accumulate 4x1 blocks, i.e. each column C
                    B0=vld1q_f32(B+B_idx);
                    C0=vfmaq_laneq_f32(C0,A0,B0,0);
                    C0=vfmaq_laneq_f32(C0,A1,B0,1);
                    C0=vfmaq_laneq_f32(C0,A2,B0,2);
                    C0=vfmaq_laneq_f32(C0,A3,B0,3);

                    B1=vld1q_f32(B+B_idx+k);
                    C1=vfmaq_laneq_f32(C1,A0,B1,0);
                    C1=vfmaq_laneq_f32(C1,A1,B1,1);
                    C1=vfmaq_laneq_f32(C1,A2,B1,2);
                    C1=vfmaq_laneq_f32(C1,A3,B1,3);

                    B2=vld1q_f32(B+B_idx+2*k);
                    C2=vfmaq_laneq_f32(C2,A0,B2,0);
                    C2=vfmaq_laneq_f32(C2,A1,B2,1);
                    C2=vfmaq_laneq_f32(C2,A2,B2,2);
                    C2=vfmaq_laneq_f32(C2,A3,B3,3);

                    B3=vld1q_f32(B+B_idx+3*k);
                    C3=vfmaq_laneq_f32(C3,A0,B3,0);
                    C3=vfmaq_laneq_f32(C3,A1,B3,1);
                    C3=vfmaq_laneq_f32(C3,A2,B3,2);
                    C3=vfmaq_laneq_f32(C3,A3,B3,3);
                }
   //Compute base index for stores
   C_idx = n*j_idx + i_idx;
   vst1q_f32(C+C_idx, C0);
   vst1q_f32(C+C_idx+n,C1);
   vst1q_f32(C+C_idx+2*n,C2);
   vst1q_f32(C+C_idx+3*n,C3);
  }
 }
}
```
### Benchmark
```
Running ./matrix_multiplication_benchmark
Run on (8 X 2600 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x8)
  L1 Instruction 64 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 32768 KiB (x1)
Load Average: 0.13, 0.33, 1.10
--------------------------------------------------------
Benchmark              Time             CPU   Iterations
--------------------------------------------------------
matrix_mul4_c        541 ns          540 ns      1296600
matrix_mul4_n        268 ns          268 ns      2624494
matrix_mul8_c       4111 ns         4109 ns       170248
matrix_mul8_n       2044 ns         2043 ns       335616

```
</details>

<!-- <details> -->
<summary><font size=5> Eigen优化实例</font></summary>

### 1. 基于SIMD的Eigen/core模块下ARM平台单/双精度浮点数开方优化
当前Eigen社区中仅有部分开发者对于浮点数开方运算在x86 powerPC mips等平台上进行了向量化优化，因此我们对其在ARM平台上进行优化.(解决了issue1933)。  
下面给出单精度浮点数开方运算向量化实现：
```cpp
// 卡马克快速开方倒数
float InvSqrt (float x){
    float xhalf = 0.5f*x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i>>1);
    x = *(float*)&i;
    // 牛顿迭代
    x = x*(1.5f - xhalf*x*x);
    return x;
}
```
```cpp
#if EIGEN_FAST_MATH

/* Functions for sqrt support packet2f/packet4f.*/
// The EIGEN_FAST_MATH version uses the vrsqrte_f32 approximation and one step
// of Newton's method, at a cost of 1-2 bits of precision as opposed to the
// exact solution. It does not handle +inf, or denormalized numbers correctly.
// The main advantage of this approach is not just speed, but also the fact that
// it can be inlined and pipelined with other computations, further reducing its
// effective latency. This is similar to Quake3's fast inverse square root.
// For more details see: http://www.beyond3d.com/content/articles/8/
template<> EIGEN_STRONG_INLINE Packet4f psqrt(const Packet4f& _x){
  Packet4f half = vmulq_n_f32(_x, 0.5f);
  Packet4ui denormal_mask = vandq_u32(vcgeq_f32(_x, vdupq_n_f32(0.0f)),
                                      vcltq_f32(_x, pset1<Packet4f>((std::numeric_limits<float>::min)())));
  // Compute approximate reciprocal sqrt.
  Packet4f x = vrsqrteq_f32(_x);
  // Do a single step of Newton's iteration. 
  //the number 1.5f was set reference to Quake3's fast inverse square root
  x = vmulq_f32(x, psub(pset1<Packet4f>(1.5f), pmul(half, pmul(x, x))));
  // Flush results for denormals to zero.
  return vreinterpretq_f32_u32(vbicq_u32(vreinterpretq_u32_f32(pmul(_x, x)), denormal_mask));
}

template<> EIGEN_STRONG_INLINE Packet2f psqrt(const Packet2f& _x){
  Packet2f half = vmul_n_f32(_x, 0.5f);
  Packet2ui denormal_mask = vand_u32(vcge_f32(_x, vdup_n_f32(0.0f)),
                                     vclt_f32(_x, pset1<Packet2f>((std::numeric_limits<float>::min)())));
  // Compute approximate reciprocal sqrt.
  Packet2f x = vrsqrte_f32(_x);
  // Do a single step of Newton's iteration.
  x = vmul_f32(x, psub(pset1<Packet2f>(1.5f), pmul(half, pmul(x, x))));
  // Flush results for denormals to zero.
  return vreinterpret_f32_u32(vbic_u32(vreinterpret_u32_f32(pmul(_x, x)), denormal_mask));
}

#else 
template<> EIGEN_STRONG_INLINE Packet4f psqrt(const Packet4f& _x){return vsqrtq_f32(_x);}
template<> EIGEN_STRONG_INLINE Packet2f psqrt(const Packet2f& _x){return vsqrt_f32(_x); }
#endif
```

Benchmarks:
```
Running ./sqrt_benchmark
Run on (8 X 2600 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x8)
  L1 Instruction 64 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 32768 KiB (x1)
Load Average: 0.25, 0.13, 0.07

float32x4_t / float32x2_t:
----------------------------
Benchmark            Time            
----------------------------
sqrt_std    49.138/36.052 ns   
psqrt_quake        28.526 ns   
psqrt_intri        29.627 ns     

float64x2_t:
----------------------------
Benchmark            Time   
----------------------------
sqrt_std           41.982 ns   
psqrt_quake        35.850 ns   
psqrt_intri        30.535 ns   
```

### 2. 基于SIMD的Eigen/core模块下ARM平台双精度浮点数指数函数优化
指数函数和对数函数在神经网络的训练中起到重要的作用，常在神经网络的激活函数和损失函数中使用，如sigmod，softmax和交叉熵损失等。Eigen在x86 powerPC mips等平台对单/双精度浮点数指数函数均实现了向量化优化，而在ARM平台上仅仅对单精度浮点数进行了向量化，以此，我们对于双精度浮点数指数函数进行优化。

```cpp
// Exponential function. Works by writing "x = m*log(2) + r" where
// "m = floor(x/log(2)+1/2)" and "r" is the remainder. The result is then
// "exp(x) = 2^m*exp(r)" where exp(r) is in the range [-1,1).
// A Pade' form  1 + 2x P(x**2)/( Q(x**2) - P(x**2) ) is used to approximate exp(r) in the basic interval
template <typename Packet>
EIGEN_DEFINE_FUNCTION_ALLOWING_MULTIPLE_DEFINITIONS
EIGEN_UNUSED
Packet pexp_double(const Packet _x)
{
  Packet x = _x;

  const Packet cst_1 = pset1<Packet>(1.0);
  const Packet cst_2 = pset1<Packet>(2.0);
  const Packet cst_half = pset1<Packet>(0.5);

  const Packet cst_exp_hi = pset1<Packet>(709.437);
  const Packet cst_exp_lo = pset1<Packet>(-709.436139303);

  const Packet cst_cephes_LOG2EF = pset1<Packet>(1.4426950408889634073599);
  const Packet cst_cephes_exp_p0 = pset1<Packet>(1.26177193074810590878e-4);
  const Packet cst_cephes_exp_p1 = pset1<Packet>(3.02994407707441961300e-2);
  const Packet cst_cephes_exp_p2 = pset1<Packet>(9.99999999999999999910e-1);
  const Packet cst_cephes_exp_q0 = pset1<Packet>(3.00198505138664455042e-6);
  const Packet cst_cephes_exp_q1 = pset1<Packet>(2.52448340349684104192e-3);
  const Packet cst_cephes_exp_q2 = pset1<Packet>(2.27265548208155028766e-1);
  const Packet cst_cephes_exp_q3 = pset1<Packet>(2.00000000000000000009e0);
  const Packet cst_cephes_exp_C1 = pset1<Packet>(0.693145751953125);
  const Packet cst_cephes_exp_C2 = pset1<Packet>(1.42860682030941723212e-6);

  Packet tmp, fx;

  // clamp x
  x = pmax(pmin(x, cst_exp_hi), cst_exp_lo);
  // Express exp(x) as exp(g + n*log(2)).
  fx = pmadd(cst_cephes_LOG2EF, x, cst_half);

  // Get the integer modulus of log(2), i.e. the "n" described above.
  fx = pfloor(fx);

  // Get the remainder modulo log(2), i.e. the "g" described above. Subtract
  // n*log(2) out in two steps, i.e. n*C1 + n*C2, C1+C2=log2 to get the last
  // digits right.
  tmp = pmul(fx, cst_cephes_exp_C1);
  Packet z = pmul(fx, cst_cephes_exp_C2);
  x = psub(x, tmp);
  x = psub(x, z);

  Packet x2 = pmul(x, x);

  // Evaluate the numerator polynomial of the rational interpolant.
  Packet px = cst_cephes_exp_p0;
  px = pmadd(px, x2, cst_cephes_exp_p1);
  px = pmadd(px, x2, cst_cephes_exp_p2);
  px = pmul(px, x);

  // Evaluate the denominator polynomial of the rational interpolant.
  Packet qx = cst_cephes_exp_q0;
  qx = pmadd(qx, x2, cst_cephes_exp_q1);
  qx = pmadd(qx, x2, cst_cephes_exp_q2);
  qx = pmadd(qx, x2, cst_cephes_exp_q3);

  x = pdiv(px, psub(qx, px));
  x = pmadd(cst_2, x, cst_1);

  // Construct the result 2^n * exp(g) = e * x. The max is used to catch
  // non-finite values in the input.
  return pmax(pldexp(x,fx), _x);
}
```

实现 pldexp 函数如下：
```cpp
template<typename Packet> EIGEN_STRONG_INLINE Packet
pldexp_double(Packet a, Packet exponent)
{
  typedef typename unpacket_traits<Packet>::integer_packet PacketI;
  const Packet cst_1023 = pset1<Packet>(1023.0);
  // return a * 2^exponent
  PacketI ei = pcast<Packet,PacketI>(padd(exponent, cst_1023));
  return pmul(a, preinterpret<Packet>(plogical_shift_left<52>(ei)));
}
```

Benchmarks:
```
Running ./pexp_benchmark
Run on (8 X 2600 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x8)
  L1 Instruction 64 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 32768 KiB (x1)
Load Average: 0.11, 0.08, 0.06
---------------------------
Benchmark           Time    
---------------------------
exp_std           83.337 ns    
pexp_simd         75.868 ns    
```

### 3. 基于SIMD的Eigen/core模块下多平台双精度浮点数对数函数优化
指数函数和对数函数在神经网络的训练中起到重要的作用，常在神经网络的激活函数和损失函数中使用，如sigmod，softmax和交叉熵损失等。完成了 NEON、SSE、AVX、AVX512模块下双精度浮点数对数函数的向量化实现。

```cpp
/* Natural logarithm
 * Computes log(x) as log(2^e * m) = C*e + log(m), where the constant C =log(2)
 * and m is in the range [sqrt(1/2),sqrt(2)). In this range, the logarithm can
 * be easily approximated by a polynomial centered on m=1 for stability.
 * Returns the base e (2.718...) logarithm of m.
 * The argument is separated into its exponent and fractional
 * parts.  If the exponent is between -1 and +1, the logarithm
 * of the fraction is approximated by
 *
 *     log(1+x) = x - 0.5 x**2 + x**3 P(x)/Q(x).
 *
 * Otherwise, setting  z = 2(x-1)/x+1),
 *                     log(x) = z + z**3 P(z)/Q(z).
 * 
 * for more detail see: http://www.netlib.org/cephes/
 */
template <typename Packet>
EIGEN_DEFINE_FUNCTION_ALLOWING_MULTIPLE_DEFINITIONS
EIGEN_UNUSED
Packet plog_double(const Packet _x)
{
  Packet x = _x;

  const Packet cst_1              = pset1<Packet>(1.0);
  const Packet cst_half           = pset1<Packet>(0.5);
  // The smallest non denormalized float number.
  const Packet cst_min_norm_pos   = pset1frombits<Packet>( static_cast<uint64_t>(0x0010000000000000ull));
  const Packet cst_minus_inf      = pset1frombits<Packet>( static_cast<uint64_t>(0xfff0000000000000ull));
  const Packet cst_pos_inf        = pset1frombits<Packet>( static_cast<uint64_t>(0x7ff0000000000000ull));

 // Polynomial Coefficients for log(1+x) = x - x**2/2 + x**3 P(x)/Q(x)
 //                             1/sqrt(2) <= x < sqrt(2)
  const Packet cst_cephes_SQRTHF = pset1<Packet>(0.70710678118654752440E0);
  const Packet cst_cephes_log_p0 = pset1<Packet>(1.01875663804580931796E-4);
  const Packet cst_cephes_log_p1 = pset1<Packet>(4.97494994976747001425E-1);
  const Packet cst_cephes_log_p2 = pset1<Packet>(4.70579119878881725854E0);
  const Packet cst_cephes_log_p3 = pset1<Packet>(1.44989225341610930846E1);
  const Packet cst_cephes_log_p4 = pset1<Packet>(1.79368678507819816313E1);
  const Packet cst_cephes_log_p5 = pset1<Packet>(7.70838733755885391666E0);

  const Packet cst_cephes_log_r0 = pset1<Packet>(1.0);
  const Packet cst_cephes_log_r1 = pset1<Packet>(1.12873587189167450590E1);
  const Packet cst_cephes_log_r2 = pset1<Packet>(4.52279145837532221105E1);
  const Packet cst_cephes_log_r3 = pset1<Packet>(8.29875266912776603211E1);
  const Packet cst_cephes_log_r4 = pset1<Packet>(7.11544750618563894466E1);
  const Packet cst_cephes_log_r5 = pset1<Packet>(2.31251620126765340583E1);

  const Packet cst_cephes_log_q1 = pset1<Packet>(-2.121944400546905827679e-4);
  const Packet cst_cephes_log_q2 = pset1<Packet>(0.693359375);

  // Truncate input values to the minimum positive normal.
  x = pmax(x, cst_min_norm_pos);

  Packet e;
  // extract significant in the range [0.5,1) and exponent
  x = pfrexp(x,e);
  
  // Shift the inputs from the range [0.5,1) to [sqrt(1/2),sqrt(2))
  // and shift by -1. The values are then centered around 0, which improves
  // the stability of the polynomial evaluation.
  //   if( x < SQRTHF ) {
  //     e -= 1;
  //     x = x + x - 1.0;
  //   } else { x = x - 1.0; }
  Packet mask = pcmp_lt(x, cst_cephes_SQRTHF);
  Packet tmp = pand(x, mask);
  x = psub(x, cst_1);
  e = psub(e, pand(cst_1, mask));
  x = padd(x, tmp);

  Packet x2 = pmul(x, x);
  Packet x3 = pmul(x2, x);

  // Evaluate the polynomial approximant , probably to improve instruction-level parallelism.
  // y = x * ( z * polevl( x, P, 5 ) / p1evl( x, Q, 5 ) );
  Packet y, y1, y2,y_;
  y  = pmadd(cst_cephes_log_p0, x, cst_cephes_log_p1);
  y1 = pmadd(cst_cephes_log_p3, x, cst_cephes_log_p4);
  y  = pmadd(y, x, cst_cephes_log_p2);
  y1 = pmadd(y1, x, cst_cephes_log_p5);
  y_ = pmadd(y, x3, y1);

  y  = pmadd(cst_cephes_log_r0, x, cst_cephes_log_r1);
  y1 = pmadd(cst_cephes_log_r3, x, cst_cephes_log_r4);
  y  = pmadd(y, x, cst_cephes_log_r2);
  y1 = pmadd(y1, x, cst_cephes_log_r5);
  y  = pmadd(y, x3, y1);

  y_ = pmul(y_, x3);
  y  = pdiv(y_, y);

  // Add the logarithm of the exponent back to the result of the interpolation.
  y1  = pmul(e, cst_cephes_log_q1);
  tmp = pmul(x2, cst_half);
  y   = padd(y, y1);
  x   = psub(x, tmp);
  y2  = pmul(e, cst_cephes_log_q2);
  x   = padd(x, y);
  x   = padd(x, y2);

  Packet invalid_mask = pcmp_lt_or_nan(_x, pzero(_x));
  Packet iszero_mask  = pcmp_eq(_x,pzero(_x));
  Packet pos_inf_mask = pcmp_eq(_x,cst_pos_inf);
  // Filter out invalid inputs, i.e.:
  //  - negative arg will be NAN
  //  - 0 will be -INF
  //  - +INF will be +INF
  return pselect(iszero_mask, cst_minus_inf,
                              por(pselect(pos_inf_mask,cst_pos_inf,x), invalid_mask));
}
```
实现pfrexp函数如下：
```cpp
template<typename Packet> EIGEN_STRONG_INLINE Packet
pfrexp_double(const Packet& a, Packet& exponent) {
  typedef typename unpacket_traits<Packet>::integer_packet PacketI;
  const Packet cst_1022d = pset1<Packet>(1022.0);
  const Packet cst_half = pset1<Packet>(0.5);
  const Packet cst_inv_mant_mask  = pset1frombits<Packet>(~0x7ff0000000000000u);
  exponent = psub(pcast<PacketI,Packet>(plogical_shift_right<52>(preinterpret<PacketI>(a))), cst_1022d);
  return por(pand(a, cst_inv_mant_mask), cst_half);
}
```
Benchmarks:  
AArch64 with NEON:
```
Run on (8 X 2600 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x8)
  L1 Instruction 64 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 32768 KiB (x1)
Load Average: 0.02, 0.04, 0.06
---------------------------
Benchmark           Time 
---------------------------
log_std           96.507 ns 
plog_simd         60.689 ns 
```

x86_64 with SSE/AVX/AVX512:
```
Run on (8 X 3000 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 1024 KiB (x4)
  L3 Unified 30976 KiB (x1)
Load Average: 0.05, 0.01, 0.00
----------------------------------------
Benchmark               Time 
----------------------------------------
log_std         30.299/32.049/32.728 ns 
plog_simd                     29.446 ns 
```



### 4. 基于指令级并行（instruction-level parallelism）对单精度浮点数指数函数进行优化
指令级并行(instruction-level parallelism)优化是指当多条指令不存在相关关系时，他们在流水线(pipeline)上可以重叠执行从而提高性能，这种指令序列存在的潜在并行性成为指令级并行。

```cpp
/* calculate y = 1 + r + p5*r**2 + p4*r**3 + p3*r**4 + p2*r**5 + p1*r**6 + p0*r**7*/
/*
  Packet r2 = pmul(r, r);
  Packet r3 = pmul(r2, r);
  Packet y = cst_cephes_exp_p0;
  y = pmadd(y, r, cst_cephes_exp_p1);
  y = pmadd(y, r, cst_cephes_exp_p2);
  y = pmadd(y, r, cst_cephes_exp_p3);
  y = pmadd(y, r, cst_cephes_exp_p4);
  y = pmadd(y, r, cst_cephes_exp_p5);
  y = pmadd(y, r2, r);
  y = padd(y, cst_1);
*/
  // Evaluate the polynomial approximant,improved by instruction-level parallelism.
  Packet y, y1, y2;
  y  = pmadd(cst_cephes_exp_p0, r, cst_cephes_exp_p1);
  y1 = pmadd(cst_cephes_exp_p3, r, cst_cephes_exp_p4);
  y2 = padd(r, cst_1);
  y  = pmadd(y, r, cst_cephes_exp_p2);
  y1 = pmadd(y1, r, cst_cephes_exp_p5);
  y  = pmadd(y, r3, y1);
  y  = pmadd(y, r2, y2);
```
Benchmarks:
```
float32x4_t pexp op on Aarch64
------------------------------
Benchmark             Time 
------------------------------
before               55.682 ns 
after                49.680 ns 
```

### 5. 解决Eigen在window平台使用msvc编译时对于向量化类型下标运算不支持触发bug（issue1991和1997）
```cpp
template<> EIGEN_STRONG_INLINE Packet2l pcast<Packet2d, Packet2l>(const Packet2d& a) {
  // using a[1]/a[0] to get high/low 64 bit from __m128d is faster than _mm_cvtsd_f64() ,but 
  // it will trigger the bug report at https://gitlab.com/libeigen/eigen/-/issues/1997 and 1991 since the 
  // a[index] ops was not supported by MSVC compiler(supported by gcc).
#if EIGEN_COMP_MSVC
  return _mm_set_epi64x(int64_t(_mm_cvtsd_f64(_mm_unpackhi_pd(a,a))), int64_t(_mm_cvtsd_f64(a)));
#elif ((defined EIGEN_VECTORIZE_AVX) && (EIGEN_COMP_GNUC_STRICT || EIGEN_COMP_MINGW) && (__GXX_ABI_VERSION < 1004)) || EIGEN_OS_QNX
  return _mm_set_epi64x(int64_t(a.m_val[1]), int64_t(a.m_val[0]));
#else
  return _mm_set_epi64x(int64_t(a[1]), int64_t(a[0]));
#endif
}
```
<!-- </details> -->