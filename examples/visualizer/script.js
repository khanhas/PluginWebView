class Loader {
  constructor() {
    this.callback = null;
  }

  load(file) {

    const request = new XMLHttpRequest();

    request.open('GET', file, true);
    request.onprogress = evt => {
      let percent = Math.floor(evt.loaded / evt.total * 100);

      this.callback(percent);
    };

    request.onload = () => {this.complete(file);};
    request.send();
  }

  progress(callback) {this.callback = callback;}

  complete() {}}



class App {
  constructor() {
      this.spheres = [];
      this.measures = [];
      this.createScene();
      this.addAmbientLight();
      this.addHemisphereLight();
      this.addDirectionalLight();
      this.addSpotLight();
      this.sphereGroup = new THREE.Object3D();
      this.scene.add(this.sphereGroup);

      // this.addFloor();
      this.positionSpheres();

      this.animate();
  }

  createScene() {
    this.scene = new THREE.Scene();
    this.scene.background = null;
    this.camera = new THREE.PerspectiveCamera(25, window.innerWidth / window.innerHeight, .1, 1000);
    this.renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
    this.renderer.setClearColor( 0xffffff, 0 ); 
    this.renderer.setSize(window.innerWidth, window.innerHeight);
    this.renderer.shadowMap.enabled = true;
    this.renderer.shadowMap.type = THREE.PCFSoftShadowMap;
    this.renderer.gammaInput = true;
    this.renderer.gammaOutput = true;

    document.body.appendChild(this.renderer.domElement);

    this.controls = new THREE.OrbitControls(this.camera, this.renderer.domElement);
    this.controls.enablePan = true;
    this.camera.position.z = 35;
    this.camera.position.y = 0;

    this.controls.enableDamping = true;
    this.controls.dampingFactor = 0.04;

    document.body.style.cursor = "-moz-grabg";
    document.body.style.cursor = "-webkit-grab";

    this.controls.addEventListener("start", () => {
      requestAnimationFrame(() => {
        document.body.style.cursor = "-moz-grabbing";
        document.body.style.cursor = "-webkit-grabbing";
      });
    });

    this.controls.addEventListener("end", () => {
      requestAnimationFrame(() => {
        document.body.style.cursor = "-moz-grab";
        document.body.style.cursor = "-webkit-grab";
      });
    });

    this.scene.rotateX(.8);
    this.scene.rotateY(.8);
  }

  createSphere(geometry, material) {
    const sphere = new THREE.Mesh(geometry, material);

    sphere.position.y = .5;
    sphere.castShadow = true;
    sphere.receiveShadow = true;

    return sphere;
  }

  addDirectionalLight() {
    this.light = new THREE.DirectionalLight(0x000000, 100);
    this.light.position.set(0, 1000, 10);
    this.scene.add(this.light);
  }

  addAmbientLight() {
    const light = new THREE.AmbientLight(0xffffff, 0);
    this.scene.add(light);
  }

  addHemisphereLight() {
    const light = new THREE.HemisphereLight(0xffffff, 0x0, .5);

    this.scene.add(light);
  }

  addSpotLight() {
    const spotLight = new THREE.SpotLight(0xffffff, .5);

    spotLight.position.set(0, 50, 20);
    spotLight.castShadow = true;
    spotLight.shadow.mapSize.width = 8000;
    spotLight.shadow.mapSize.height = spotLight.shadow.mapSize.width;

    this.scene.add(spotLight);
  }


  animate() {
    this.renderer.render(this.scene, this.camera);

    this.drawWave();

    requestAnimationFrame(this.animate.bind(this));
  }

  addGrid() {
    const size = 70;
    const divisions = 70;

    const gridHelper = new THREE.GridHelper(size, divisions);
    gridHelper.position.set(0, 0, 0);
    gridHelper.material.opacity = .6;
    gridHelper.material.transparent = true;
    this.scene.add(gridHelper);
  }

  radians(degrees) {
    return degrees * Math.PI / 180;
  }

  positionSpheres() {
    const width = 8;
    const geometry = new THREE.SphereGeometry(.3, 32, 32);




    for (let i = 0; i < width; i++) {
      for (let j = 0; j < width; j++) {
        const c= new THREE.Color( 1, 0, (i*width+j)/64 )
        const material = new THREE.MeshPhongMaterial({
          color: c,
          specular: c,
          shininess: 100,
          emissive: 0x0,
          side: THREE.DoubleSide });

        let sphere = this.createSphere(geometry, material);

        this.sphereGroup.add(sphere);

        sphere.position.x = i;
        sphere.position.z = j;

        this.spheres.push(sphere);
        let mes = RainmeterAPI.getMeasure("MeasureBand" + (i*width+j));
        this.measures.push(mes);
      }
    }
    this.sphereGroup.position.set(-4, 0, -4);
  }

  addFloor() {
    const material = new THREE.MeshPhongMaterial({
      color: 0xffffff,
      specular: 0x0000,
      shininess: 100,
      flatShading: true,
      side: THREE.BackSide });


    this.object = new THREE.Mesh(new THREE.PlaneGeometry(70, 70), material);
    this.object.position.set(0, 0, 0);
    this.object.receiveShadow = true;
    this.object.rotation.x = this.radians(90);
    this.scene.add(this.object);
  }


  drawWave() {
      for (let i = 0; i < 64; i++) {
        const p = this.measures[i].getNumber();
        const s = this.spheres[i];
        const z = s.position;

        TweenMax.to(z, .2, {
          y: p * 6 });

      }
  }

  onResize() {
    const ww = window.innerWidth;
    const wh = window.innerHeight;
    this.camera.aspect = ww / wh;
    this.camera.updateProjectionMatrix();
    this.renderer.setSize(ww, wh);
  }}


const app = new App();

window.addEventListener("resize", app.onResize.bind(app));