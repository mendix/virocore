package com.viromedia.releasetest.tests;


import android.graphics.Color;

import com.viro.renderer.jni.AmbientLight;
import com.viro.renderer.jni.Box;
import com.viro.renderer.jni.DirectionalLight;
import com.viro.renderer.jni.Material;
import com.viro.renderer.jni.Node;
import com.viro.renderer.jni.Quaternion;
import com.viro.renderer.jni.Renderer;
import com.viro.renderer.jni.Vector;
import com.viro.renderer.jni.Camera;
import com.viromedia.releasetest.ViroReleaseTestActivity;

import org.junit.Test;

import java.util.Arrays;

public class ViroCameraTest extends ViroBaseTest {
    private Camera mCamera;
    private Renderer mRenderer;
    private float mRotationAngle;
    @Override
    void configureTestScene() {

        ViroReleaseTestActivity activity = (ViroReleaseTestActivity)mActivity;
        mRenderer = activity.getViroView().getRenderer();
        final float[] lightDirection = {0, 0, -1};
        final AmbientLight ambientLightJni = new AmbientLight(Color.WHITE, 1000.0f);
        mScene.getRootNode().addLight(ambientLightJni);

        DirectionalLight directionalLight = new DirectionalLight(Color.WHITE, 2000.0f, new Vector(0, 0, -1));
        mScene.getRootNode().addLight(directionalLight);

        final Material material = new Material();
        material.setDiffuseColor(Color.BLUE);
        material.setLightingModel(Material.LightingModel.BLINN);

        // Creation of ViroBox
        final Node node = new Node();
        Box box = new Box(1, 1, 1);

        node.setGeometry(box);
        final float[] boxPosition = {0, 0, -5};
        node.setPosition(new Vector(boxPosition));
        box.setMaterials(Arrays.asList(material));
        mScene.getRootNode().addChildNode(node);

        mCamera = new Camera();
        mRotationAngle = 0.0f;
        mScene.getRootNode().setCamera(mCamera);
        mRenderer.setPointOfView(mScene.getRootNode());
    }

    @Test
    public void testCamera() {
        testCameraPosition();
        testCameraOrbitMode();
        testCameraRotationQuaternion();
        testCameraRotationEuler();
    }

    private void testCameraOrbitMode() {
        mMutableTestMethod = null;
        mCamera.setRotationType(Camera.RotationType.ORBIT);
        mCamera.setOrbitFocalPoint(new Vector(0, 0, -10));
        assertPass("Camera should orbit around box", ()->{
        mCamera.setRotationType(Camera.RotationType.STANDARD);});
    }

    private void testCameraPosition() {
        mMutableTestMethod = () -> {
            Vector position = mCamera.getPosition();
            if(position.z > -5f) {
                mCamera.setPosition(new Vector(0, 0, position.z + .2f));
            }
        };
        assertPass("Camera position moves back.", ()->{mCamera.setPosition(new Vector(0, 0, 0));});
    }

    private void testCameraRotationQuaternion() {
        mMutableTestMethod = () -> {
            mRotationAngle+=5.0f;
            Quaternion quaternion = ViroCameraTest.toQuaternion(0, Math.toRadians(mRotationAngle), 0);
            mCamera.setRotation(quaternion);
        };

        assertPass("Camera rotates upwards (via Quaternion).", () ->{mCamera.setRotation(ViroCameraTest.toQuaternion(0, 0, 0));});
    }

    private void testCameraRotationEuler() {
        mMutableTestMethod = () -> {
            mRotationAngle+=5.0f;
            mCamera.setRotation(new Vector(0, Math.toRadians(mRotationAngle), 0));
        };

        assertPass("Camera rotates to the left (via Euler angles).", ()->{
            mCamera.setRotation(new Vector());
            mRotationAngle = 0;
        });
    }

    // TODO: VIRO-2166 Move to Quaternion class
    private static Quaternion toQuaternion(double pitch, double roll, double yaw)
    {
        Quaternion q = new Quaternion();
        // Abbreviations for the various angular functions
        double cy = Math.cos(yaw * 0.5);
        double sy = Math.sin(yaw * 0.5);
        double cr = Math.cos(roll * 0.5);
        double sr = Math.sin(roll * 0.5);
        double cp = Math.cos(pitch * 0.5);
        double sp = Math.sin(pitch * 0.5);

        q.w = (float)((cy * cr * cp) + (sy * sr * sp));
        q.x = (float)((cy * sr * cp) - (sy * cr * sp));
        q.y = (float)((cy * cr * sp) + (sy * sr * cp));
        q.z = (float)((sy * cr * cp) - (cy * sr * sp));
        return q;
    }
}
