ROOT_PATH=<absolute-path>
LIBRARY_NAME=libBulletSoftBody
OUTPUT_NAME=BulletSoftBody


xcodebuild -create-xcframework -library $ROOT_PATH/armv7_arm64/$LIBRARY_NAME.a \
-headers $ROOT_PATH/include/$OUTPUT_NAME \
-library $ROOT_PATH/x86_64/$LIBRARY_NAME.a \
-headers $ROOT_PATH/include/$OUTPUT_NAME \
-output $ROOT_PATH/$OUTPUT_NAME.xcframework
