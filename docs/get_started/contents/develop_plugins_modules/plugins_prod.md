### Distribute your plugin

We recommand create archives of your plugins, especially if you want to upload it into the Infinite Garage.

However if the plugin has no problem itself it can run on any compatible proijects by simply being in the .vx/plugins folder.

### Automate the distribution

We recommand creating scripts to automate the distribution process.
In order to do that, create a "scripts" folder at the root of your plugin.

Then create a build.sh inside the "scripts" folder to allow building on Linux and MacOS

```sh
rm -rf ../dist
rm -rf ../prod

mkdir -p ../dist
mkdir -p ../prod

MODULE_JSON_PATH="../plugin.json"
NAME=$(jq -r .name $MODULE_JSON_PATH)
VERSION=$(jq -r .version $MODULE_JSON_PATH)

FOLDER_NAME="$NAME-$VERSION"

mkdir -p "../dist/$FOLDER_NAME"
cp -r ../src ../dist/$FOLDER_NAME/
cp ../plugin.json ../dist/$FOLDER_NAME/


TAR_NAME="../prod/$FOLDER_NAME.tar.gz"
tar -czf "$TAR_NAME" -C ../dist "$FOLDER_NAME"

echo "Archive created : $TAR_NAME"

CHECKSUM_NAME="../prod/$FOLDER_NAME.tar.gz.sha256"
sha256sum "$TAR_NAME" | awk '{print $1}' > "$CHECKSUM_NAME"
```

And create a build.bat inside the "scripts" folder to allow building on Microsoft Windows


```bat
Bat here
```

Then you simply need to execute theses script to build your plugin.

Note: A plugin is cross platform, so building on Linux will make your plugin ready for Microsoft Windows and Macos (and vice-versa).