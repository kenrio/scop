# scop (3D model viewer)

![3Dモデルビューアデモ](/resources/3d-model-viewer-1.gif "3Dモデルビューア")

**C++とOpenGLで.obj形式の3Dモデルをリアルタイム描画するビューア**

OBJパースから頂点データの構築、GPUへの転送、シェーダによる描画までのレンダリングパイプラインを、外部ライブラリ（GLM / stb_image / Assimp）を使用せずに実装し、描画処理の理解を目的として構築した。

また、ImGuiによるUIを導入し、モデル・テクスチャの切り替えや描画状態（テクスチャ・ライティング・UV）の制御をリアルタイムで行えるインタラクティブなビューアとした。

機能：

* OBJモデル表示
* OpenGLによるラスタライズ描画
* テクスチャマッピング（平面マッピング / OBJ UV）
* ワイヤーフレーム / ライティング / 法線ベクトル可視化トグル
* キーボード・マウス入力

## 実行環境

* macOS (Apple Silicon)
* C++17対応コンパイラ
* OpenGL 3.3以降
* GLFW

`ImGui`および`glad`はリポジトリ内に同梱しているため、別途インストールは不要

## 実行方法

ライブラリのインストール：
```
$ brew install glfw
```
ビルド：
```
$ make
```
実行：
```
$ ./scop						# デフォルトモデルを表示
$ ./scop <path/to/model.obj>	# 指定したOBJモデルを表示
```
## 操作方法

* キーボード：

	* `WASD / QE`：モデル移動
	* `T`：テクスチャ表示トグル
	* `L`：ライティングトグル
	* `U`：平面マッピング / OBJ UVトグル
	* `F`：ワイヤーフレームトグル
	* `N`：法線ベクトルトグル
	* `Space`：自動回転トグル
	* `R`：ビューリセット
	* `Esc`：終了

* マウス：

	* `左ドラッグ`：回転
	* `右ドラッグ`：パン
	* `スクロール`：ズーム

## 苦労した点

* BMPファイルフォーマットの仕様対応

	テクスチャ読み込みのため、`stb_image`を使用せずにBMPローダーを自作した。実装当初はテクスチャが表示されない、色が反転している、画像が上下逆に表示されるといった現象が発生した。BMPファイルの仕様を調べて、以下2点に起因することを特定し対応した：

	* ピクセルデータがBGR順で格納されているため、読み込み時にRGB順に並び替える
	* BITMAPINFOHEADERのbiHeightが正の値の場合は`bottom-up`、負の値の場合は`top-down`であるため、ヘッダ情報に合わせてピクセルデータの読み込み順を切り替える
		* `src/Texture.cpp`：BMPファイルのBGR配列、top-down形式をRGB・bottom-up形式のOpenGL仕様に変換

## 工夫・力をいれた点

* OpenGL/GLSLによるレンダリングパイプラインの構築

	OBJファイルのテキストデータを描画可能な形式でGPUに渡すために **モデルデータ** → **頂点配列** → **GPUバッファ** → **シェーダ処理** という処理フローに分離した。CPU側で頂点データの生成・整形、GPU（シェーダ）側で座標変換・ライティング・ピクセル処理という、OpenGLパイプラインに沿った役割分担で実装した。
	* `src/ObjParser.cpp`：v / vt / vn / f 行の解析、fan triangulationによる三角形分割
	* `src/ModelViewer.cpp`：頂点データの構築、CPU→GPUへのデータ転送、描画フロー管理
	* `shaders/vertex.glsl`：Model / View / Projection による頂点座標変換

* 3D数学ライブラリの自作

	GLMを使用せずに3D数学の基礎を理解するために、 Vec3 / Vec4 / Mat4 のクラスを実装。translate / rotate / scale / perspective / lookAt 行列を含む。Mat4は内部データを`float data[4][4]`で保持し、OpenGLの列優先メモリレイアウトに合わせて格納することで、	`glUniformMatrix4fv()`への転送にtransposeを不要とした。
	* `src/Mat4.cpp`：translate / rotate / scale / perspective / lookAt 行列演算

* 複数テクスチャマッピング方式を統合したシェーダ

	OBJ由来のUV座標と平面マッピング用UVの2方式を実装。フラグメントシェーダで`mix()`による連続補間を用い、2方式間を滑らかに切り替え可能にした。同じ方法でテクスチャ表示とライティングのトグルにも適用し、シェーダ全体で一貫した制御方式とした。
	* `shaders/fragment.glsl`：テクスチャ、ライティングなどの描画状態のリアルタイム切り替え

## 参考にしたソースファイル

* LearnOpenGL (https://learnopengl.com)
	* OpenGL/GLSLの基本概念、Shader / Textureクラス設計、VAO/VBO/EBOの使い方

* OpenGL reference (https://registry.khronos.org/OpenGL-Refpages/gl4/)
	* `glDrawArrays()`, `glUniformMatrix4fv()` 等の仕様
* songho.ca (https://www.songho.ca/opengl/gl_projectionmatrix.html)
	* 透視投影行列など3D数学の行列計算

* Qiita - Bitmapファイルフォーマット徹底解説 (https://qiita.com/ImagingSolAkira/items/30fd3727afa3076b8050)
	* BMPヘッダ構造、BGR/RGB変換、top-down/bottom-up対応
