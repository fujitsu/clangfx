
# clangfx導入手順書

本書では、以下のように略称で記述しています。  
Technical Computing Suite             : TCS  
Red Hat Enterprise Linux              : RHEL   
clangfx(px)コンパイラ                 : clangfx  

注意
- 本書の導入手順はTCSがインストールされた環境向けとなります。
- 本書ではTCSの世代版数としてtcsds-1.2.34を想定しています。  
  世代版数が更新されている場合は版数を読み替えてください。  
  世代版数は以下のディレクトリ配下から確認することができます。  
  `/opt/FJSVxtclanga/tcsds-x.x.x`

## 1. 本書について

本書では、LLVM13.0.0をベースとして以下の機能を追加した富岳向けC/C++コンパイラ"clangfx"の導入手順について記載します。

- SIMD化機能改善
- 富士通MPI連携
- 富士通ツール連携
- 富岳環境クロスコンパイラ

## 2. clangfx導入時の注意事項

clangfx導入には以下のパッケージが必要です。パッケージが適用されていない場合は、システムに合ったRHELのインストールディスクから、不足しているパッケージを適用してください。  
システムの状況により、以下に記載したパッケージ以外のパッケージも必要となる場合があります。必要に応じて不足パッケージも適用してください。

[RHEL8]
 - CMake >= 3.13.4
 - GCC >= 5.1.0
 - python >= 3.6
 - zlib >= 1.2.3.4
 - GNU Make >= 3.79

LLVMが要求するパッケージの一覧は以下のLLVM公式サイトから確認することができます。  
https://llvm.org/docs/GettingStarted.html#software

## 3. clangfx導入手順

### 3-1. Fujitsu GitHubからclangfxソースのダウンロード
富士通が公開している以下のGitHubからclangfxのソースをダウンロードし、任意のディレクトリに配置してください。  
https://github.com/fujitsu/clangfx

### 3-2. clangfxビルド
####   3-2-1. ネイティブコンパイラ
aarch64マシン上でclangfxネイティブコンパイラを以下の手順でビルドします。

3-1でダウンロードしたclangfxのソースディレクトリへ移動し、`own_build.sh`を実行します。  
`$ ./own_build.sh`

※ビルドができない場合は、利用するGCCのバージョンを確認してください。  
libc++のビルドを含む場合はgcc 11.0以上、libc++を含まない場合はgcc 5.1.0以上が必要です。

ビルドが完了すると、clangfxソースツリーのトップディレクトリに`install`ディレクトリが作成され、コマンド・ライブラリ等が配置されます。

	$ ls ./install/
	bin/  include/  lib/  lib64/  libexec/  share/

後述するクロスコンパイラの手順を実施すると`install`ディレクトリが上書きされるため、`install`ディレクトリのディレクトリ名のリネーム等により対処してください。

####   3-2-2. クロスコンパイラ
x86_64マシン上でclangfxクロスコンパイラを以下の手順でビルドします。

3-1でダウンロードしたclangfxのソースディレクトリへ移動し、`cross_build.sh`を実行します。  
`$ ./cross_build.sh`

※ビルドができない場合は、利用するGCCのバージョンを確認してください。  
clangfxのクロスコンパイラでは、gcc 8.2.0ベースのdevkitを使用しています。  
devkitとの非互換を考慮し、クロスコンパイラのビルドにはgcc 8の使用を推奨します。  
gcc 8を使用する場合、clangfxのクロスコンパイラでは、libc++は制限となります。

3-2-1と同様にビルドが完了すると、clangfxソースツリーのトップディレクトリに`install`ディレクトリが作成され、コマンド・ライブラリ等が配置されます。

### 3-3. clangfxコンパイラ環境の構築
####  3-3-1. ディレクトリ構成
3-1でダウンロードしたclangfxソースツリーのトップディレクトリにある、`clangfx`ディレクトリを任意の場所にコピーしてください。  
このディレクトリが以降で構築するclangfxコンパイラ環境のベースになります。  
以降の説明では、clangfxディレクトリをコピーした任意のディレクトリを"dir"とします。

ネイティブ/クロスコンパイラ2種類の環境を構築する場合は、以下のようにclangfxディレクトリを2つに分けて以降の手順を行ってください。

	$ cp -rp clangfx/ cross_clangfx/  # クロスコンパイラ用のclangfxディレクトリ
	$ mv clangfx/ native_clangfx/     # ネイティブコンパイラ用のclangfxディレクトリ

同じclangfxディレクトリ内にネイティブ/クロスの環境が混在しないようにしてください。

clangfxディレクトリは以下の構成となっています。

	clangfx
	├ clang-comp/           # ビルドしたネイティブまたはクロスコンパイラ格納ディレクトリ
	├ bin/                  # コンパイラドライバ格納ディレクトリ
	├ lib64/                # ライブラリ格納ディレクトリ
	└ include/              # 富士通ヘッダ格納ディレクトリ

####  3-3-2. コンパイラ本体の配置
3-2でビルドした`install`ディレクトリ配下のファイル(`bin`,`include`,`lib`,`lib64`,`libexec`,`share`ディレクトリ)を、`clang-comp`ディレクトリ配下にコピーしてください。

####  3-3-3. 富士通TCSライブラリの配置
富士通TCSライブラリを`lib64`ディレクトリにコピーしてください。  

	$ cp -rp /opt/FJSVxtclanga/tcsds-1.2.34/lib64/* [任意のディレクトリ]/clangfx/lib64/

以下の手順でclangfx/lib64内のファイルを削除してください。

	$ cd dir/clangfx/lib64
	$ rm -f libc++.a libc++.so.1 libc++abi.a libc++abi.so.1 libc++experimental.a libc++.so libc++.so.1.0 libc++abi.so libc++abi.so.1.0

また、`clangfx/lib64`内の`libclang_rt.profile-aarch64.a`のリンクを貼りなおしてください。

	$ unlink libclang_rt.profile-aarch64.a
	$ ln -s ../clang-comp/lib64/clang/13.0.0/lib/linux/libclang_rt.profile-aarch64.a .

####  3-3-4. 富士通TCSヘッダの配置
富士通ツール(プロファイラ)のヘッダファイル(`fipp.h`, `fapp.h`)が入った`fj_tool`ディレクトリを、`include`ディレクトリにコピーしてください。

例)  

	cp -rp /opt/FJSVxtclanga/tcsds-1.2.34/clang-comp/include/external/fj_tool dir/clangfx/include/

####  3-3-5. ネイティブコンパイラ環境のライブラリの配置
環境変数`LD_LIBRARY_PATH`は`clangfx/lib64`に通っている想定のため（3-4節参照）、ネイティブコンパイラのライブラリを`clangfx/lib64`に移動してください。  

	$ cp -rp dir/clangfx/clang-comp/lib64/* dir/clangfx/lib64

####  3-3-6. クロスコンパイラ環境のライブラリの配置
クロスコンパイラを使用するには、ビルドしたネイティブコンパイラのライブラリが必要です。3-2-1の手順でaarch64マシン上でネイティブコンパイラをビルドしてください。  
クロスコンパイラビルド時と同様に、gcc 8でのビルドを推奨します。`own_build.sh`のcmakeオプション`-DLLVM_ENABLE_PROJECTS`から`libcxx;libcxxabi`を削除してください。

以下の手順で、ビルドしたネイティブコンパイラのファイルをクロスコンパイラ環境へコピーします。

	※ dir/install/がビルドしたネイティブコンパイラのパス
	$ cp -rp dir/install/lib64/* dir/clangfx/lib64
	$ cp -rp dir/install/lib64/clang/13.0.0/lib dir/clangfx/clang-comp/lib64/clang/13.0.0 

####  3-3-7. clangfxコンパイルドライバについて
`clangfx/bin`ディレクトリ配下のファイルがclangfxコンパイラのドライバファイルとなります。
clangfxコンパイラは以下のコンパイラドライバを実行することでコンパイルを行います。
   
	clangfx      - ネイティブコンパイラ(C言語)
	clangfx++    - ネイティブコンパイラ(C++言語)
	clangfxpx    - クロスコンパイラ(C言語)
	clangfxpx++  - クロスコンパイラ(C++言語)

また、同じディレクトリにclangfxコンパイラを利用する場合の、MPIプログラムのコンパイルコマンドが格納されています。

	mpiclangfx      - ネイティブコンパイル用mpiコマンド(C言語)
	mpiclangfx++    - ネイティブコンパイル用mpiコマンド(C++言語)
	mpiclangfxpx    - クロスコンパイル用mpiコマンド(C言語)
	mpiclangfxpx++  - クロスコンパイル用mpiコマンド(C++言語)

mpiclangfxコマンドは富士通MPIの`mpifcc`を実行するため、`mpifcc`コマンドを
参照できる状態となっている必要があります。

上記のドライバファイルを利用したコンパイル方法は、別資料の"利用者ガイド"を参照してください。

### 3-4. パスの追加
clangfxコンパイラのコマンド/ライブラリのパスを環境変数に登録します。
ディレクトリ構成例と、環境変数の設定例を以下に示します。

以下はログインノードと実機ノード両方から参照できるディレクトリ上で、ネイティブ/クロスコンパイラ両方の環境を構築した例です。

ディレクトリ構成例)

	└ dir/
	   └ own_clangfx/    # ネイティブコンパイラ環境のトップディレクトリ
	       ├ clang-comp/           # ビルドしたネイティブコンパイラ格納ディレクトリ
	       ├ bin/                  # コンパイラドライバ格納ディレクトリ
	       ├ lib64/                # ライブラリ格納ディレクトリ
	       └ include/              # 富士通ヘッダ格納ディレクトリ
	   └ cross_clangfx/  # クロスコンパイラ環境のトップディレクトリ
	       ├ clang-comp/           # ビルドしたクロスコンパイラ格納ディレクトリ
	       ├ bin/                  # コンパイラドライバ格納ディレクトリ
	       ├ lib64/                # ライブラリ格納ディレクトリ
	       └ include/              # 富士通ヘッダ格納ディレクトリ

ネイティブコンパイラ実行時の環境変数の設定例)   

	CLANGFX=dir/own_clangfx
	export PATH=${CLANGFX}/bin:/opt/FJSVxtclanga/tcsds-1.2.34/bin:${PATH}
	export LD_LIBRARY_PATH=${CLANGFX}/lib64:${LD_LIBRARY_PATH}

クロスコンパイラ実行時の環境変数の設定例)

	CLANGFX=dir/cross_clangfx
	export PATH=${CLANGFX}/bin:/opt/FJSVxtclanga/tcsds-1.2.34/bin:${PATH}
	export LD_LIBRARY_PATH=${CLANGFX}/lib64:${LD_LIBRARY_PATH}

## 4. 注意事項

### 4-1. clangfxのビルドについて
clangfxのビルドに使用したマシンとclangfxを実行するマシンが異なる場合、ビルド時のgccのライブラリ
(`libstdc++.so`など)が、clangfxを実行する環境で必要になる場合があります。必要に応じてライブラリを
clangfxを実行するマシンにコピーしてパスを通す等の対処をしてください。

クロスコンパイラでの翻訳時はx86_64バイナリのgccライブラリを`LD_LIBRARY_PATH`に設定してください。  
ネイティブコンパイラでの翻訳時、および翻訳したプログラムの実行時は、aarch64バイナリのgccライブラリを`LD_LIBRARY_PATH`に設定してください。

例)  

	$ export LD_LIBRARY_PATH=[gccのインストールディレクトリ]/lib64:${LD_LIBRARY_PATH}

## 5. 修正履歴

| 版数 | 修正日     | 修正概要               | 修正者 |  
| ---- | ---- | ---- | ----- |  
| 1.0  | 2022/03/17 | 初版作成               | 富士通 |  

以上
