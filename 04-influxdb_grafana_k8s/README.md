# InfluxDB + Grafana Kubernetes scripts

## Create a secret to store InfluxDB and Grafana auth information

Note: if you deploy to GKE, skip this part for now, as you'll first need to configure your cluster before being able to run this script.

```
kubectl create secret generic influxdb-grafana \
  --from-literal=influxdb-user=admin \
  --from-literal=influxdb-password=passw0rd \
  --from-literal=grafana-user=admin \
  --from-literal=grafana-password=passw0rd
```


## Deploy locally

Make sure you have `minikube` installed and started.

```sh
kubectl create -f k8s/
minikube service list
```

Once services are exposed, go to the Grafana dasboard (`minikube service grafana --url`) add the following InfluxDB data source URL: `http://influxdb:8086`.


## Deploy to GKE

Navigate to the [Google Kubernetes Engine](https://console.cloud.google.com/kubernetes/list) section of the web console and wait for the system to initialize.  
Then, start a console and run the following:

```sh
# Set defaults for the gcloud command-line tool
export ZONE=<zone> # e.g. europe-west1-b see https://cloud.google.com/compute/docs/regions-zones/#available
export PROJECT_ID=<project-id>
gcloud config set project $PROJECT_ID
gcloud config set compute/zone $ZONE

# Create a GKE cluster with one n1-standard-1 node:
gcloud container clusters create influxdb-grafana \
                --num-nodes 1 \
                --machine-type n1-standard-1 \
                --zone $ZONE

# Create the secret, if you haven't already
# (see the section above)

# Deploy InfluxDB and Grafana to Kubernetes
kubectl create -f k8s/

# See the services external IPs / ports
kubectl get services
```

Then, go to `http://<grafana service external ip>:3000` and add an InfluxDB data source:  
* URL: http://influxdb:8086
* Database: iot


## Cleaning up

If you want to delete everything, run the following:

```sh
# Delete k8s services, deployments, persistent volumes
kubectl delete -f k8s/

# Delete the secret
kubectl delete secret influxdb-grafana

# Delete the GKE cluster
gcloud container clusters delete influxdb-grafana

# Delete compute engine disks
gcloud compute disks list
gcloud compute disks delete <disk-name>
```
