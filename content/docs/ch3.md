# Chapter 3: Using Particle Webhooks with IFTTT & azure IoT Central

| **Project Goal**            | Use Particle Webhooks and Integrations to connect your app to IFTTT and Azure IoT Central.                                                                                        |
| --------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **What you’ll learn**       | Working with the Particle IoT Rules Engine; Piping sensor data into FileMaker. |
| **Tools you’ll need**       | Particle Workbench, Particle IoT Rules Engine Account, a Particle Argon and the Grove Starter Kit for Particle Mesh                                                                                                           |
| **Time needed to complete** | 60 minutes                                                                                                                                                                |

In this session, we're going to explore the Particle IoT Rules Engine, before moving onto 3rd party integrations with FileMaker.

## Introducing the Particle IoT Rules Engine

The Particle IoT Rules Engine is an IF/THEN workflow builder tailor-made for IoT use cases. It's meant to enable anyone on an IoT project team create conditional logic using a visual interface.

We suggest [visiting the Particle Docs](https://docs.particle.io/tutorials/iot-rules-engine/quickstart/) for an in-depth overview on the Particle IoT Rules Engine. In the meantime, the key concepts to understand are that you'll work with the Rules Engine by using units of functionality (called nodes) from a visual menu of available nodes (called the palette) to form a connected sequence of steps (called a flow), where messages are passed between each step.

In this section of the lab, we'll create a flow to ingest and transform sensor data from your Continental.iot device, build dashboards to visualize that data, and add UI to trigger basic interactions back on your embedded device.

### Publishing environmental sensor data

The Particle IoT Rules Engine is connected to the Particle Device Cloud, which means that anythig you make cloud-accessible via Pub/Sub, Functions or Variables (all of which we covered in the last lab), can be leveraged by the Rules Engine. Let's make a quick modification to our project from the last lab to publish environmenal data collected by thr Grove Barmoeter sensor.

1. First, we'll need to install a new library: the `JsonParserGenerator`, which will help us build JSON objects with multiple fields to post to the Particle Cloud. Open the Particle Workbench Command Palette (CMD/CTRL+SHIFT+P) and select the "Install Library" option.
2. In the input field, type "JsonParserGeneratorRK" and hit enter.
3. Once the library is installed, add an include statement to the top of your project

```cpp
#include "JsonParserGeneratorRK.h"
```

4. To build the JSON object for posting, we'll create a new function called `postSensorData` that will take all three barometer sensor readings, format those into a JSON object, and then call `Particle.publish` to post them to the Device Cloud.

```cpp
void postSensorData(int temp, int pressure, int altitude)
{
  JsonWriterStatic<256> jsonWriter;

  {
    JsonWriterAutoObject obj(&jsonWriter);

    jsonWriter.insertKeyValue("temperature", temp);
    jsonWriter.insertKeyValue("pressue", pressure);
    jsonWriter.insertKeyValue("altitude", altitude);
  }

  Particle.publish("environment/latest", jsonWriter.getBuffer());
}
```

5. Now, in the `loop`, add a call to this new function, just after the `Serial` print commands.

```cpp
postSensorData(temp, pressure, altitude);
```

6. Flash these changes to your device and open the device dashboard in the Particle Console. Once the device comes back online, you should see events posting every 10 seconds. Click on one of the entries to see the formatted JSON result.

![](./images/03/json-sensors.png)

Now you're ready to start using the Particle IoT Rules Engine!

### Creating your first Rules Engine Flow

1. Open your Rules Engine instance. The URL and login information should have been provided by your instructor.

2. Click the plus (+) tab at the top right of the screen to create a new flow.

![](./images/03/newFlow.png)

3. Double-click on the flow to change its default name and give it a description.

![](./images/03/edit-description.png)

Once created, your new flow will just be an empty, gridded palette. Let's start adding nodes to it and build a flow!

![](./images/03/emptyflow.png)

## Ingesting and displaying data in the Rules Engine

### Subscribing to Particle Cloud events

You may have already noticed that the node palette on the left side of the Rules Engine window contains five Particle nodes. These nodes provide easy access to Particle primitives (pub/sub, functions, variables), as well as any public endpoint in the Particle API. Let's use the Particle subscribe node to ingest our environmental data.

![](./images/03/particlenodes.png)

1. Click and drag the `subscribe` node and drop it anywhere on the flow at the center of your screen.

![](./images/03/subscribe-node.png)

2. Double-click on the node to open its configuration window.

![](./images/03/subscribe-config.png)

3. Give the node a name, like "Environment Sensors."

![](./images/03/name-sub.png)

4. Next, we need to set-up a mechanism for the Rules Engine flow to safely access your device data. Click on the pencil icon next to the "Auth" setting row and you'll see a window that instructs you to create an OAuth client, which we'll do now.

![](./images/03/node-oauth.png)

5. Open [the Particle console](https://console.particle.io) and click on the fingerprint icon to navigate to the authentication section.

![](./images/03/console-oauth.png)

6. Click the "New Client" button on the Authentication screen.

![](./images/03/new-client-button.png)

7. When creating a Particle OAuth client, there are a number of supported client types, depending on whether you need server-based authentication, self-contianed auth for a mobile app, or your own custom approach. For connecting to Rules Engine flows, you'll want to use the "Two-Legged Auth (Server)" option. Make sure that option is selected, give the client a name, and click the "Get Client ID and Secret" button.

![](./images/03/oauth-client.png)

8. Once the client is created, you'll be provided with the client ID and secret, both of which you'll need in the next step. Make sure you copy the client secret and keep it somewhere safe as you won't be able to access it again once you click the "I've Copied my Secret" button!

![](./images/03/client-created.png)

9. Copy your client ID and secret and enter them in the inputs back in the Rules Engine configuration screen. Click "Update" once you're done.

![](./images/03/client-config.png)

10. Once your Auth information is set, you can complete the rest of the settings for the subscribe node. First, enter the event nameto subscribe to, which we set as `environment/latest` in the last section. Then,set your device name or ID.

![](./images/03/sub-config.png)

:::tip
If you don't know your device name or ID, you can find both in the console, at the top of your device dashboard screen.

![](./images/03/id-name.png)
:::

### Using the Debug Node and deploying flows

11. To make sure that your sensor data is properly wired up to the Rules Engine, you can use the debug node. Find it under the "Output" section of the palette, or search for it using the search input at the top left of the screen.

![](./images/03/debug-node.png)

12. Drag the debug node onto your flow. Connect it to the subscribe node by clicking on the grey square on the right side of the subscribe node, and dragging the line that appears to the box on the left of the debug node.

![](./images/03/connect-debug.png)

13. Now your flow is ready to test, but it won't run until its deployed. To do that, click the "Deploy" button at the top right of the screen.

![](./images/03/deploy-sub.png)

Once deployed, you'll see a success message like the one below.

![](./images/03/deployed.png)

14. To see debug messages, you'll need to open the debug tab in the flow sidebar. If it's not already open, you can do so by dragging on the sidebar handle on the right of the Rules Engine window.

![](./images/03/flow-sidebar.png)

15. To open the debug tab, click the bug icon in the flow info panel.

![](./images/03/debug-tab.png)

16. Once open, you should see the output of the subscribe node as new messages come in every ten seconds.

![](./images/03/debug-subs.png)

Nice work, you've wired your Continental.iot project up to the Rules Engine! Now let's build some simple dashboards using the built-in tools.

## Transforming Data and Building Dashboards in the Rules Engine

In addition to creating workflows with the Rules Engine, you can also create dashboards to visualize device data. In this section, we'll look at adding gauges, charts and some UI to trigger basic interactivity. 

### Creating Charts

Charts are useful for viewing data across a quantifiable sequence, or period like time. In this section, we'll create a single line graph that displays readings from our barometer sensor, over time.

#### Formatting input data for display

1. The first thing we need to do is format the sensor data from our publish event. The data somes in as a JSON string, but we need to convert it to a JSON object in order to access the data for our dashboards. Thankfully, the Rules Engine provides a `json` node we can use for just this purpose. Find the node in the "Function" section, or by searching for "json".

![](./images/03/json.png)

2. Drag the JSON node to your flow, and connect it to the "Environment Sensors" subscrobe node.

![](./images/03/tojson.png)


3. Double-click on the `json` node to open its properties. Keep the defaults, and give the node a name, like "To JSON." Click Done.

![](./images/03/jsonprops.png)

4. Next, we need to format the data for our chart. Oftentimes, you'll need to transform or shape data in order to feed it into future steps in a workflow. The Rules Engine provides a general-purpose `function` node for this purpose. This node accepts a message from a previous step, and executes user-defined JavaScript that transforms the input data into an output message to be passed along to the next step in the flow. Find the function node under the Fuction palette group, or by searching for function. Note: that the general-purpose function is NOT the Particle function node, which you will also see in a search.

![](./images/03/functionnode.png)

5. Drag the function node onto your flow and connect it to the "To JSON" node.

![](./images/03/emptyfnnode.png)

6. Double-click on the function node and give it a name like "format temp for chart." In the function textarea, add some JavaScript code to extract the temperature from the incoming message and format it for the graph we'll create

```js
var tempMsg = {};

tempMsg.payload = msg.payload.temperature;
tempMsg.topic = "Temperature";

msg = tempMsg;

return msg;
```

::: tip
You might wonder at this point why we're creating a new object and only passing along the temperature. Dashboard nodes need the incoming to [conform to a consistent format](https://github.com/node-red/node-red-dashboard/blob/master/Charts.md) in order for data to be parsed and displayed using a given UI component. If you want to display more than a single series of data on a chart, each series needs to be labeled with a `topic` and the payload must be a single numeric value. Thus, to show three sets of data on a single chart, we have to pass along three distinct messages.
:::

7. Once you've added the snippet above, click "Done." Your flow should now look like this.

![](./images/03/firstfn.png)

8. Now let's add function nodes for pressure and altitude, and format their data accordingly. Drag two more funciton nodes to the flow and connect them to the "To JSON" node.

9. Double-click on the first function node, give it a name like "format pressure for chart" and add the following code to format the pressure data for the chart.

```js
var pressureMsg = {};

pressureMsg.payload = msg.payload.pressure;
pressureMsg.topic = "Pressure";

msg = pressureMsg;

return msg;
```

10. Double-click on the second function node, give it a name like "format altitude for chart" and add the following code to format the pressure data for the chart.

```js
var altMsg = {};

altMsg.payload = msg.payload.altitude;
altMsg.topic = "Altitude";

msg = altMsg;

return msg;
```

11. Now, you should have three functions connected to your JSON object, and the flow should look something like this.

![](./images/03/allfns.png)

Now we're ready for our chart! 

#### Adding the Dashboard UI

As with everything else in the Rules Engine, dashboard functionality is accessible via a collection of nodes. The Particle IoT Rules Engine provides a number of such nodes under the "Dashboard" palette group, including charts, gauges, and input controls like buttons and switches.

![](./images/03/dashboard.png)

1. Drag the "chart" node to the flow and connect each function node from the previous section to its input.

![](./images/03/newchart.png)

2. Double-click on the chart node to open its properties panel. When creating a dashboard, you need to instruct the Rules Engine where to display the data. It's possible to create multiple dashboards across a Rules Engine instance, and we want to create a single view for our sensor data. Open the "Group" dropdown and select "Add new ui_group." Then, click on the pencil icon.

![](./images/03/choosegroup.png)

3. In the "Add new dashboard group config node" give the group a name, like "Continental Sensors." Then, open the "Tab" dropdown, select the "Add new ui_tab" option, and click the pencil icon.

![](./images/03/choosetab.png)

4. In the "Edit dashboard tab node," give the tab a name, like "Continental." You can also modify the Icon if you wish (see the links on this properties pane for more info.). Click "Update" when done.

![](./images/03/edittab.png)

5. You'll be navigated back to the "Add new dashboard group config node" tab and your new tab should be selected. Click "Add" to continue.

6. With the new UI options set, you can now configure the chart. In the "Edit chart node" screen, set a label, provide a blank label like "No data available" for cases where data is pending, and give the node a name.

![](./images/03/configchart.png)

7. Now, your flow should look like this. Click the "Deploy" button to deploy your changes and start charting!

![](./images/03/flowwithchart.png)

8. Once the deploy completes, you can open your new dashboard. Open the settings sidebar and look for the chart icon at the top right. Click it to open the Dashboard menu.

![](./images/03/dashboardtab.png)

9. Click the launch icon at the top right of the dashobard tab to launch the dashboard for your Rules Engine Instance.

![](./images/03/opendash.png)

10. When the dashboard opens, you'll see your new chart, which will update as data streams in, real time.

![](./images/03/chartview.png)

Now lets add a gauge to our dashboard.

### Creating Gauges

Gauges are useful for showing a single piece of quantifiable data at a point in time, and positioning that data between boundary conditions (min, max), or labeling the data based on where it falls within a range. For example, we can use a gauge to display the current temperature and where it falls within, safe, warning, and danger ranges. Let's do that for the temperature reading from our barometer now.

1. Find the `gauge` node in the Dashboard, drag it onto the flow and connect it to the "format temp for chart" function node.

![](./images/03/gauge.png)

2. Double-click on the node to open its property pane. Make sure the Group dropdown is set to the same group you set for the chart. Set the range inputs to a min of `0` and max of `110`. For the "Colour gradient" add the values `80` and `100` in the empty sector inputs. This tells the gauge to display green for values under 80, yellow for values between 80, and 100 for anything over 100. Finally, give the gauge node a name.

![](./images/03/gaugeprops.png)

3. Click "Deploy" to deploy your changes, and open your dashobard tab again. You should immediately see your gauge under the graph you created in the last section.

![](./images/03/gaugeandchart.png)

And if your temp sensor detects a value above 80, the gauge color will change.

![](./images/03/gaugeyellow.png)

### Adding UI for Interactivity

Now that we've explored using the Rules Engine a bit, let's add some interactivity. So far, we've used the Rules Engine to respond to incoming data. But we can also use the UI featutes of the Rules Engine dashboard to trigger actions on our device. In this section, we'll add a button that triggers a function call, when pressed.

#### Adding a readSensors function to firmware

Let's add a new function to our Argon firmware that will read the barometer sensor values, on demand.

1. In the `setup` function, add a new `Particle.function` primitive.

```cpp
Particle.function("readSensors", readSensors);
```

2. Next, add the `readSensors` function, which we can code by taking the existing `loop` code that reads and posts sensor data. We'll also add a conditional so we can log when a sensor check is triggered by the dashboard.

```cpp
int readSensors(String arguments)
{
  temp = (int)cToF(bmp.readTemperature());
  pressure = (int)pToHg(bmp.readPressure());
  // Update to reflect current pressure at sea-level
  altitude = (int)bmp.readAltitude(1035.7);

  if (!arguments.equals(""))
  {
    Serial.printlnf("Read request from: %s", arguments.c_str());
  }

  Serial.printlnf("Temp: %i", temp);
  Serial.printlnf("Pressure: %i", pressure);
  Serial.printlnf("Altitude: %i", altitude);

  postSensorData(temp, pressure, altitude);

  return 1;
}
```

3. Flash this new firmware to your device and return to the Rules Engine.

#### Triggering a function call from the Rules Engine

1. To trigger a function call via user interaction, we can add the `button` dashboard node. Find the node and drag it to your flow, underneath the main sensors portion of the flow. Since this will be user-triggered, we don't want to connect it to the existing flow.

![](./images/03/addbutton.png)

2. Double-click the button to open its properties window. Add a label of "Read from sensors" and a name of "Read Sensors." Click "Done."

3. Next, drag a Particle function node from the Particle palette group, and connect it to the button.

![](./images/03/buttonandfn.png)

4. Double-click the function node to open its property pane. Give it a name like "Call readSensors." Make sure the Auth dropdown has the same selected value as your subscribe node from earlier in this lab. For the function field, type "readSensors," and provide an argument value of "dashboard." finally, set the device ID and click "Done."

![](./images/03/functionprops.png)

5. Deploy your latest changes and open the dashboard UI page. You'll see a read from sensors button at the bottom.

![](./images/03/buttonui.png)

Clicking the button will trigger the `readSensors` function on your device, which you can verify by opening a serial terminal and veiewing the output after each click.

![](./images/03/buttonterminal.png)

## Connecting Rules Engine Flows to Azure IoT Central

The Particle IoT Rules Engine is a great tool for creating workflows and simple visualizations, but sometimes you need to backhaul your data to a cloud provider for additional processing or routing. Thankfully, the Rules Engine makes this easy. 

In the next labs, you'll explore Azure IoT Central and IoT Hub further. For now, let's connect your Continental.iot project to Azure IoT Hub using the Rules Engine. First, you'll need to set-up an IoT Hub instance.

### Setting up an Azure IoT Hub

1.  Sign up for an [Azure Account](https://azure.microsoft.com/en-us/get-started/), or sign in if you already have one.

![](./images/04/azureacct.png)

2.  In the dashboard, click "Create a resource." Click "Internet of Things," then "IoT Hub" at the top of the list.

![](./images/04/resourcelist.png)

3.  Create a new resource group named `workshop-group` and give the hub a name. The name must be unique across all of Azure, but you'll receive a notification if the name you selected is not unique.

![](./images/04/iothubdetails.png)

4.  Click "Review + create".

![](./images/04/reviewcreate.png)

5.  Click "Create".

![](./images/04/hubcreate.png)

6.  You'll get a notification that your IoT Hub is being deployed, which may take a few minutes. Once done, pin the hub to your dashboard.

![](./images/04/deploymentinprogress.png)

7.  Click "Go to resource" to open the hub.

![](./images/04/deploymentsucceeded.png)

8.  Click "Shared access policies". We'll create a policy to allow the Particle Device Cloud to stream events into the hub.

9.  Give the policy the name `workshop-policy`, select all four permissions, then click "Create."

![](./images/04/shared-access-policy.png)

10. Open the policy you just created and copy the "Primary key."

![](./images/04/policykey.png)

Now we can use connect our IoT Hub instance to our Rules Engine flow.

### Using the Azure IoT Hub node

1. Find the "Azure IoT Hub" node under the Azure palette group. Drag it to the workflow surface and connect it to the "To JSON" node.

![](./images/03/azureiot.png)

2. Double-click on the Azure IoT Hub node and set its properties to use `mqtt` as the transport protocol and the name of your IoT Hub as the host.

![](./images/03/hubprops.png)


3. Deploy the latest changes to your flow.

### Viewing IoT Hub Events

With the event set up, everything should be piping into the Azure IoT hub. We can verify using the [iothub-explorer command line utility](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-explorer-cloud-device-messaging?WT.mc_id=7061727469636c65).

1.  Install the iothub-explorer using the instructions [here](https://github.com/azure/iothub-explorer).

2.  Navigate to your IoT Hub in the azure portal, and return to your shared access policy, `workshop-policy`. Copy the Connection string. You'll need this to authenticate with the iothub-explorer.

![](./images/04/copy-conn-string.png)

3.  Open a terminal window and type in the following command, replacing the `<device-id>` with the ID of your Electron and pasting the Shared access policy connection string in place of `<connection-string>` after login. Make sure to leave the quotation marks in place.

```bash
iothub-explorer monitor-events <device-id> --login "<connection-string>"
```

If you've authenticated correctly, you'll see the message "Monitoring events from device `<device-id>`...". After a moment, you should see events streaming into the Azure IoT Hub!

![](./images/04/iothubexplorer.gif)

Once you're streaming device data into the Azure IoT Hub, you can pipe the data into Azure table storage, set-up streaming analytics to transform the data, create Azure Web Apps, reports with Power BI, use Azure Machine Learning, and more! In the next section, we'll set-up a streaming analytics job and visualize our data in Power BI.

