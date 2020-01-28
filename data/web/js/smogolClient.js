import $ from "./jquery.module.js";

export class SmogolClient
{
	_handlers = {"online": [], "offline": [], "newData": []};
	_online = false;

	constructor(baseUrl)
	{
		this.baseUrl = baseUrl;
	}

	isOnline()
	{
		return this._online;
	}

	onOnline(handler)
	{
		this._handlers["online"].push(handler);
	}

	onOffline(handler)
	{
		this._handlers["offline"].push(handler);
	}

	onNewData(handler)
	{
		this._handlers["newData"].push(handler);
	}

	start()
	{
		this._intervalHandlerId = setTimeout(this._intervalHandler.bind(this), 1000);
	}

	stop()
	{
		// clearInterval(this._intervalHandlerId);
	}

	_intervalHandler()
	{
		this._foo()
			.then(data => 
				{
					this._handleNewData(data)
					this._intervalHandlerId = setTimeout(this._intervalHandler.bind(this), 1000);
				})
			.catch(error => 
				{
					this._handleCommError(error)
					this._intervalHandlerId = setTimeout(this._intervalHandler.bind(this), 1000);
				});
	}

	_handleNewData(data)
	{
		this._goOnline();
		this._newDataNotification(data);
	}

	_handleCommError(error)
	{
		console.log(error);
		this._goOffline();
	}

	_foo()
	{
		let that = this;
		return new Promise(
			function(resolve, reject)
			{
				$.ajax({
					dataType: "json",
					url: that._getUrl("sensor", "getRecent"),
					timeout: 5000,
					success: data => resolve(data),
					error: error => reject(error)
			});
		});
	}

	_getUrl(module, method)
	{
		return this.baseUrl + "/" + module + "/" + method;
	}

	_goOnline()
	{
		if (this._online)
		{
			return;
		}

		this._online = true;
		this._handlers["online"].forEach(handler => handler());
	}

	_goOffline()
	{
		if (!this._online)
		{
			return;
		}

		this._online = false;
		this._handlers["offline"].forEach(handler => handler());
	}

	_newDataNotification(data)
	{
		this._handlers["newData"].forEach(handler => handler(data));
	}
}
